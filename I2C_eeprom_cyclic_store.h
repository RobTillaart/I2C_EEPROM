#pragma once
//
//    FILE: I2C_eeprom_cyclic_access.h
//  AUTHOR: Tomas Hübner
// VERSION: 1.0.0
// PURPOSE: Supplemental utility class for I2C_EEPROM library
//


#include <I2C_eeprom.h>

/**
 * @brief This is a utility class for using an eeprom to store a simple
 * data structure.
 * 
 * The purpose of the utility is to extend the life of an eeprom memory by
 * rotating the writes over different pages to reduce the number of writes
 * to any individual page.
 * It does this by partitioning the memory into slots (of continuous pages)
 * big enough to contain the data structure. When data is written to a slot
 * it is given a header with a
 * version number.
 * On initialization the slots are scanned for the one with the highest
 * version number which is then used for subsequent reads.
 * Whenever data is written the version number is incremented and the next
 * slot in sequence is used (or the first slot if going past the end).
 *  
 * Note that the data is stored in binary form which means it should not be
 * expected that the eeprom can be moved between architectures. Data stored
 * in an eeprom will also become invalid if the data structure is changed
 * either manually or due to changed optimization settings.
 *
 * If the data structure has changed or if the eeprom contains other data it
 * must first be formatted with a call to format().
 * 
 * Finally, since the version number is a long word, this class will
 * *derail and fail* to add new versions past 4294967295 writes.
 * 
 * @tparam T the type of the data structure to store, should only contain
 * **value** members and no constructor/destructor nor other
 * methods/functions - e g a pure DTO.
 */
template<typename T>
class I2C_eeprom_cyclic_store
{
    public:
        /**
        * Initializes the instance with the I2C_eeprom object to use
        * This call searches the eeprom for the latest written version and
	* sets the current slot accordingly.
        * 
        * @param eeprom  the instance of I2C_eeprom to use
        * @param pageSize is the number of bytes in each write page
        * @param totalPages specifies the total number of pages to use. By
	* specifying a number that i sless than the available pages will
	* exclude the remaining pages from being used.
        * @return true if initialization succeeds, false otherwise
        */
        bool begin(I2C_eeprom & eeprom, uint8_t pageSize, uint16_t totalPages)
        {
            _eeprom = &eeprom;
            _pageSize = pageSize;
            _totalPages = totalPages;
            auto bufferSize = sizeof(uint32_t)+sizeof(T);
            _bufferPages = bufferSize/_pageSize+(bufferSize%_pageSize ? 1 : 0);

            return (_bufferPages < _totalPages) && initialize();
        };

        /**
         * Formats the eeprom by writing the max version number to each slot 
         * 
         * This must be done if the eeprom already contains data or if the
	 * structure of the data stored changes.
         */
        void format()
        {
            // Reset the EEPROM by writing a ~0 into all pages
            for(uint16_t slot = 0; slot < _totalPages; slot++)
            {
                _eeprom->writeBlock(slot*_pageSize, (uint8_t *) "\xff\xff\xff\xff", 4);
            }

            _isEmpty = true;
            _currentSlot = 0;
            _isInitialized = true;
        }

        /**
         * Read data from the eeprom into an instance. The data is read from
	 * the current slot of the eeprom.
         * 
         * @param buffer the reference to an object to read data for
         * @return true if data was read successfully, false otherwise
         */
        bool read(T& buffer) const { return read(&buffer); }

        /**
         * Read data from the eeprom into a memory instance. The data is read
	 * from the current slot of the eeprom.
         * @param buffer the instance to read data into
         * @return true if data was read successfully, false otherwise
         */
        bool read(T* buffer) const
        {
            if(!_isInitialized)
                return false;

            if(_isEmpty)
                return false;

            return _eeprom->readBlock((_currentSlot*_bufferPages*_pageSize)+sizeof(_currentVersion), (uint8_t *) buffer, sizeof(T)) == sizeof(T);
        }

        /**
         * Write data of object to the next slot in the eeprom. This updates
	 * the current slot of this instance.
         * 
         * @param buffer the reference to the object to write data for
         * @return true if data was written successfully, false otherwise
         */
        bool write(T& buffer) { return write(&buffer); }

        /**
         * Write data of object to the next slot in the eeprom. This updates
	 * the current slot of the instance.
         *
         * @param buffer the pointer to the instance to write data for
         * @return true if data was written successfully, false otherwise
         */
        bool write(T* buffer)
        {
            if(!_isInitialized)
            {
                return false;
            }

            if(_isEmpty)
            {
                _currentSlot = 0;
                _currentVersion = 0;
            }
            else
            {
                _currentSlot++;
                _currentVersion++;

                // Wrap around to start if going past end of alotted region
                uint16_t maxSlots = _totalPages/_bufferPages;
                if(_currentSlot > maxSlots)
                    _currentSlot = 0;
            }

            auto buffer_length = sizeof(_currentVersion)+sizeof(T);
            uint8_t tmp[buffer_length];

            memcpy(tmp, &_currentVersion, sizeof(_currentVersion));
            memcpy(tmp+sizeof(_currentVersion), buffer, sizeof(T));

            auto success = _eeprom->writeBlock(_currentSlot*_bufferPages*_pageSize, tmp, buffer_length) == 0;

            if(success)
                _isEmpty = false;

            return success;
        }   

    private:
        uint8_t _pageSize;
        uint16_t _bufferPages;
        uint16_t _totalPages;
        uint16_t _currentSlot;
        uint32_t _currentVersion;
        bool _isInitialized = false;
        bool _isEmpty = false;
        I2C_eeprom * _eeprom;

        bool initialize()
        {
            // Find the current block

            uint16_t startSlot, probeSlot, endSlot;
            uint32_t current, probe;

            startSlot = 0;
            endSlot = (_totalPages/_bufferPages)-1; // Index of last slot
            probeSlot = startSlot+((endSlot-startSlot)/2); // Midway between start and end

            _eeprom->readBlock(0,  (uint8_t *) &current, sizeof(uint32_t));

            if(current == ~0UL)
            {
                // Memory is blank
                _isEmpty = true;
                _currentSlot = 0;
                _currentVersion = 0;
                _isInitialized = true;
                return true;
            }

            while(startSlot != probeSlot)
            {
                _eeprom->readBlock(probeSlot*_pageSize*_bufferPages, (uint8_t *) &probe, sizeof(uint32_t));

                if(probe == ~0UL || probe <= current)
                {
                    // 1. Nothing has been written to the memory at Probe
                    // 2. The slots have the same timestamp, this shouldn't happen, treat as if Probe slot hasn't been written
                    // 3. Probe is older that Start, change End to Probe
                    endSlot = probeSlot;
                    probeSlot = startSlot+((endSlot-startSlot)/2);
                    continue;
                }
                else if(probe > current)
                {
                    // 1. Probe is later than Start. change Start to Probe
                    startSlot = probeSlot;
                    probeSlot = startSlot+((endSlot-startSlot)/2);
                    current = probe;
                }
            }

            _currentSlot = startSlot;
            _currentVersion = current;
            _isEmpty = false;
            _isInitialized = true;

            return this->_isInitialized;
        }
};

