This is wip sfv checker
It is not stable i wouldn't recommend you to use.


It uses CRCpp library for crc32 calculation. I shaved this library to fit my needs.
https://github.com/d-bahr/CRCpp

What QtSfv does for now:
 - Uses Qt6/5 for it's gui and cross platform compability
 - Utilizes threads in order to speed up crc32 calculation
 - Uses C++20

I planned these features for QtSfv
 - Better UX/UI
 - Faster CRC32 calculations with less memory requirements
 - Shell extension for win32