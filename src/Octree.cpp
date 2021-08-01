#include "Octree.hpp"

const uint32_t Octree::BitCount[] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

Octree::Octree()
{
    // _tree = {0x0004FF00};
    // It would appear leaf mask is the left 8 bits?
    // _tree = {0x0004FFFF,
    //          0x00008200, 0x00008200, 0x00008200, 0x00008200,
    //          0x00008200, 0x00008200, 0x00008200, 0x00008200};
    _treeSize = 1 + (8 * 9); // Why the extra 8? One of the rows must be being skipped somehow
    _tree = {
        0x0004FFFF,
        0x0023FFFF, 0x0042FFFF, 0x0061FFFF, 0x0080FFFF,
        0x009FFFFF, 0x00BEFFFF, 0x00DDFFFF, 0x00FCFFFF,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,

        0x00008200, 0x00008200, 0x00008200, 0x00008200,
        0x00008200, 0x00008200, 0x00008200, 0x00008200,
    };

#if 0
    _tree = {
        0x43333,
        0x10FFFF,
        0x8C5555,
        0xBC0F0F,
        0xF00404,
        0x208000,
        0x20C000,
        0x24A000,
        0x28F000,
        0x348800,
        0x38CC00,
        0x44AA00,
        0x50EC00,
        0xC61919FE,
        0xCCECD2FE,
        0xD7FD45FE,
        0xC619FFFE,
        0xC61AE5FE,
        0xD045E77E,
        0xD6A5F77E,
        0xCCEF2CFE,
        0xD7FAB9FE,
        0xA8DC43FE,
        0x83E319FE,
        0xA68D62FE,
        0xA9B7337E,
        0xB4EDDA7E,
        0xB766B07E,
        0x8FB0DF7E,
        0x96C1447E,
        0x8C0AA1FE,
        0x9B1F11FE,
        0xC61AE5FE,
        0x29B333FE,
        0x59E119FE,
        0x3A61597E,
        0x1B9F86FE,
        0x104000,
        0x105000,
        0x144400,
        0x185500,
        0xD7FD45FE,
        0xDC35FFFE,
        0xD7FAB9FE,
        0x4C49E7E,
        0x45F86FE,
        0xC494EFE,
        0x1B349E7E,
        0xFAAE07E,
        0x167EA97E,
        0x100800,
        0x100C00,
        0x140A00,
        0x181F00,
        0x47FD45FE,
        0x4C348BFE,
        0x5308D2FE,
        0x47FD45FE,
        0x47FEB9FE,
        0x4FC5E67E,
        0x55357BFE,
        0x4DAEC7FE,
        0x530B2C7E,
        0x4FC5E67E,
        0x40500,
        0x59E1FFFE,
        0x59E2E5FE};
#endif
#if 0
    _tree = {
        0x4FFFF,
        0x207F7F,
        0x2B05555,
        0x4183333,
        0x57C1111,
        0x6188F8F,
        0x7F80505,
        0x9040303,
        0x9F40101,
        0x1C8080,
        0x24F0F0,
        0x6CF0F0,
        0xB0FFFF,
        0x144EAEA,
        0x1A03F3F,
        0x2007F7F,
        0x4A000,
        0xC61919FE,
        0xC61919FE,
        0x10C000,
        0x144000,
        0x14FC00,
        0x28FC00,
        0xCCECD2FE,
        0xCCECD2FE,
        0xD3C48BFE,
        0xC61919FE,
        0xCCECD2FE,
        0xC61919FE,
        0xCCECD2FE,
        0xC61919FE,
        0xCCECD2FE,
        0xD3C48BFE,
        0xD3C48BFE,
        0xD3C48BFE,
        0xD3C48BFE,
        0xD3C48BFE,
        0xD3C48BFE,
        0x108000,
        0x10FA00,
        0x242000,
        0x24FA00,
        0xC61919FE,
        0xC61919FE,
        0xC61919FE,
        0xC61919FE,
        0xC61919FE,
        0xC61919FE,
        0xC61919FE,
        0xC619FFFE,
        0xC619FFFE,
        0xC61AE5FE,
        0xC619FFFE,
        0xC619FFFE,
        0xC61AE5FE,
        0xC61AE5FE,
        0x20E000,
        0x28F000,
        0x34B000,
        0x3CF000,
        0x481F00,
        0x580B00,
        0x604F00,
        0x700E00,
        0xCCECD2FE,
        0xC61919FE,
        0xC61919FE,
        0xCCECD2FE,
        0xD3C48BFE,
        0xD24937FE,
        0xD7FD45FE,
        0xC619FFFE,
        0xC619FFFE,
        0xCCEF2CFE,
        0xD045E77E,
        0xD7FAB9FE,
        0xCCEF2CFE,
        0xD7FAB9FE,
        0xC61919FE,
        0xCCECD2FE,
        0xC61919FE,
        0xC61919FE,
        0xC61919FE,
        0xD3C48BFE,
        0xD7FD45FE,
        0xD7FD45FE,
        0xC619FFFE,
        0xC61AE5FE,
        0xC61AE5FE,
        0xCCEF2CFE,
        0xC61AE5FE,
        0xDC35FFFE,
        0xD3C773FE,
        0xD7FAB9FE,
        0x148800,
        0x18FE00,
        0x308800,
        0x348800,
        0x387700,
        0x845C797E,
        0xAFFC8C7E,
        0x845C797E,
        0x845C797E,
        0x845C797E,
        0x845D86FE,
        0x845D86FE,
        0x845D86FE,
        0x845D86FE,
        0xAFFC8C7E,
        0xAFFC8C7E,
        0x845D86FE,
        0x8AC9FFFE,
        0x845D86FE,
        0x845D86FE,
        0x845D86FE,
        0xAFFC8C7E,
        0xAFFC8C7E,
        0x845D86FE,
        0x18CC00,
        0x24CC00,
        0x300100,
        0x300300,
        0x34CC00,
        0x40CC00,
        0xA68D62FE,
        0xAA962BFE,
        0xAFFC8C7E,
        0xAFFDFFFE,
        0xA59EA5FE,
        0xA59EA5FE,
        0xACC6A67E,
        0xACC6A67E,
        0xC61919FE,
        0xD3C48BFE,
        0xA59EA5FE,
        0xB335597E,
        0xAFFDFFFE,
        0xB335597E,
        0xB335597E,
        0xAFFDFFFE,
        0xACC6A67E,
        0xAFFDFFFE,
        0xACC6A67E,
        0x1CAA00,
        0x281700,
        0x34AA00,
        0x404500,
        0x48AA00,
        0x540100,
        0x54AA00,
        0x845C797E,
        0x893544FE,
        0x845D86FE,
        0x8AC9FFFE,
        0x845C797E,
        0xC61919FE,
        0xC61919FE,
        0x845D86FE,
        0x8FB0DF7E,
        0x9B98797E,
        0x952DFFFE,
        0x952DFFFE,
        0xC61AE5FE,
        0x9B98797E,
        0x9B9D86FE,
        0x8AC9FFFE,
        0x8AC9FFFE,
        0x8AC9FFFE,
        0x8FFE617E,
        0x845D86FE,
        0x8FFE617E,
        0x952DFFFE,
        0x8FFE617E,
        0x952DFFFE,
        0x10C0C0,
        0x2CF0F0,
        0x78DDDD,
        0xF0AAAA,
        0x8F400,
        0x184000,
        0xD3C48BFE,
        0xD3C48BFE,
        0xD3C48BFE,
        0xD3C48BFE,
        0xD7FD45FE,
        0xD7FD45FE,
        0x10F500,
        0x245000,
        0x28F500,
        0x3C5000,
        0xD7FD45FE,
        0xDC35FFFE,
        0xD7FD45FE,
        0xD7FD45FE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xD7FAB9FE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xDC35FFFE,
        0xDC35FFFE,
        0x184400,
        0x1C3B00,
        0x2CD500,
        0x3CCC00,
        0x48A300,
        0x545D00,
        0xACC6A67E,
        0xACC6A67E,
        0xA59EA5FE,
        0xA59EA5FE,
        0xD3C48BFE,
        0xACC6A67E,
        0xAFFB73FE,
        0x4C49E7E,
        0x8D4A67E,
        0xAFFB73FE,
        0xAC9FFFE,
        0xAC9FFFE,
        0xACC6A67E,
        0xAFFB73FE,
        0xAFFB73FE,
        0xAFFB73FE,
        0xACC6A67E,
        0xAFFB73FE,
        0x45E78FE,
        0x45E78FE,
        0x45E78FE,
        0x45E78FE,
        0xAC9FFFE,
        0x45E78FE,
        0x45E78FE,
        0x10F500,
        0x24F500,
        0x38BB00,
        0x4CEE00,
        0xFFCC3FE,
        0xFFCC3FE,
        0xFFD9E7E,
        0xFFD9E7E,
        0xFFD9E7E,
        0xFFD9E7E,
        0xFFCC3FE,
        0xFFCC3FE,
        0xFFD9E7E,
        0xFFD9E7E,
        0xFFD9E7E,
        0xFFD9E7E,
        0x45E78FE,
        0xAC9FFFE,
        0xFFD9E7E,
        0x45E78FE,
        0xAC9FFFE,
        0xAC9FFFE,
        0xFFD9E7E,
        0x1B9E78FE,
        0x152DFFFE,
        0xFFD9E7E,
        0x1B9E78FE,
        0x152DFFFE,
        0x102020,
        0x1CF0F0,
        0x6CBBBB,
        0xD0CFCF,
        0x4B000,
        0xC61AE5FE,
        0xC61AE5FE,
        0xC61AE5FE,
        0x10FB00,
        0x28F300,
        0x3C3000,
        0x403000,
        0xC61AE5FE,
        0xCCEF2CFE,
        0xCCEF2CFE,
        0xC61AE5FE,
        0xCCEF2CFE,
        0xC61AE5FE,
        0xCCEF2CFE,
        0xD3C773FE,
        0xD3C773FE,
        0xD3C773FE,
        0xD3C773FE,
        0xD3C773FE,
        0xD3C773FE,
        0xCCEF2CFE,
        0xCCEF2CFE,
        0xD3C773FE,
        0xD3C773FE,
        0x182000,
        0x18DF00,
        0x302200,
        0x342200,
        0x38DD00,
        0x4C2200,
        0x9B9D86FE,
        0x9B98797E,
        0x9B98797E,
        0x9B9D86FE,
        0x9B98797E,
        0x9B9D86FE,
        0x9B9D86FE,
        0x9B9D86FE,
        0x9B98797E,
        0x2FFC8C7E,
        0x9B9D86FE,
        0x9B9D86FE,
        0x9B9D86FE,
        0x9B9D86FE,
        0x9B9D86FE,
        0x9B9D86FE,
        0x2FFC8C7E,
        0x2FFC8C7E,
        0x2FFC8C7E,
        0x2FFC8C7E,
        0x180400,
        0x180800,
        0x18FF00,
        0x347300,
        0x44BB00,
        0x58FF00,
        0xC61AE5FE,
        0xD3C773FE,
        0x24A0D87E,
        0x259AA67E,
        0x266D4F7E,
        0x2A962C7E,
        0x2FFC8C7E,
        0x2FFDFFFE,
        0x3335597E,
        0x2FFDFFFE,
        0x259AA67E,
        0x259AA67E,
        0x2CC6A67E,
        0x2CC6A67E,
        0x2CC6A67E,
        0x3335597E,
        0x3335597E,
        0x2FFDFFFE,
        0x3335597E,
        0x3335597E,
        0x3335597E,
        0x2CC6A67E,
        0x2CC6A67E,
        0x2FFDFFFE,
        0x2CC6A67E,
        0x3335597E,
        0x2CC6A67E,
        0x2FFDFFFE,
        0x2CC6A67E,
        0x83030,
        0x287777,
        0x8F100,
        0x185000,
        0xD7FAB9FE,
        0xD7FAB9FE,
        0xD7FAB9FE,
        0xD3C773FE,
        0xD3C773FE,
        0xD7FAB9FE,
        0xD3C773FE,
        0x18CC00,
        0x247500,
        0x343100,
        0x3C8800,
        0x405700,
        0x503300,
        0x259AA67E,
        0x259AA67E,
        0x2CC6A67E,
        0x2FFB73FE,
        0x1720A5FE,
        0x1B349E7E,
        0x152DFFFE,
        0x152DFFFE,
        0x2FFB73FE,
        0x259AA67E,
        0x2CC6A67E,
        0x2CC6A67E,
        0x2FFB73FE,
        0x1B9E78FE,
        0x1B9E78FE,
        0x152DFFFE,
        0x1B9E78FE,
        0x1B9E78FE,
        0x1B9E78FE,
        0x2CC6A67E,
        0x2FFB73FE,
        0x2FFB73FE,
        0x2FFB73FE,
        0x148E8E,
        0x54EFEF,
        0xD4AFAF,
        0x148F0F0,
        0x1900F0F,
        0x100800,
        0x108800,
        0x14F700,
        0x2C0E00,
        0xB335597E,
        0x8AC9FFFE,
        0x8AC9FFFE,
        0xAFFC8C7E,
        0xB335597E,
        0x845D86FE,
        0x83E319FE,
        0xBA61597E,
        0x88A31FFE,
        0x4C348BFE,
        0x4C348BFE,
        0x47FD45FE,
        0x47FD45FE,
        0x1CCC00,
        0x28CC00,
        0x343100,
        0x3C1000,
        0x3C0C00,
        0x40CF00,
        0x54CF00,
        0xB335597E,
        0xB335597E,
        0xBA61597E,
        0xBA61597E,
        0xAFFDFFFE,
        0xAFFDFFFE,
        0xBA61597E,
        0xB83A3B7E,
        0xB335597E,
        0xBA61597E,
        0x4C348BFE,
        0x4C348BFE,
        0x4C348BFE,
        0x5308D2FE,
        0x4C348BFE,
        0x4C348BFE,
        0x4C348BFE,
        0x4C348BFE,
        0x4C348BFE,
        0x4C348BFE,
        0x4C348BFE,
        0x5308D2FE,
        0x4C348BFE,
        0x5308D2FE,
        0x4C348BFE,
        0x5308D2FE,
        0x18AA00,
        0x245000,
        0x28AA00,
        0x345000,
        0x38AF00,
        0x4CAF00,
        0x8FFE617E,
        0x8FFE617E,
        0x8FFE617E,
        0x8FFE617E,
        0x8FFF3BFE,
        0x8FFF3BFE,
        0x8FFE617E,
        0x8FFE617E,
        0x8FFE617E,
        0x8FFE617E,
        0x8FFF3BFE,
        0x8FFF3BFE,
        0x43C5FFFE,
        0x47FD45FE,
        0x43C5FFFE,
        0x43C5FFFE,
        0x47FD45FE,
        0x43C5FFFE,
        0x43C5FFFE,
        0x43C5FFFE,
        0x43C5FFFE,
        0x43C5FFFE,
        0x43C5FFFE,
        0x47FEB9FE,
        0x10F100,
        0x20B000,
        0x28F000,
        0x34E000,
        0x4C348BFE,
        0x47FD45FE,
        0x4C348BFE,
        0x47FD45FE,
        0x47FD45FE,
        0x5308D2FE,
        0x5308D2FE,
        0x59E119FE,
        0x43C5FFFE,
        0x43C5FFFE,
        0x47FEB9FE,
        0x47FEB9FE,
        0x59E1FFFE,
        0x530B2C7E,
        0x59E2E5FE,
        0x100A00,
        0x140F00,
        0x200A00,
        0x241F00,
        0x4C348BFE,
        0x47FD45FE,
        0x5308D2FE,
        0x59E119FE,
        0x4E29327E,
        0x59E119FE,
        0x47FEB9FE,
        0x47FEB9FE,
        0x4FC5E67E,
        0x59E1FFFE,
        0x530B2C7E,
        0x59E2E5FE,
        0x4FC5E67E,
        0x85D5D,
        0x5CFFFF,
        0x144C00,
        0x1CFB00,
        0x344400,
        0x380400,
        0x384D00,
        0xAFFB73FE,
        0x45E78FE,
        0xB766B07E,
        0x45E78FE,
        0x45E78FE,
        0x45E78FE,
        0x45F86FE,
        0x45F86FE,
        0x59E119FE,
        0x45F86FE,
        0x45E78FE,
        0x45F86FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x202000,
        0x20FB00,
        0x388000,
        0x38FE00,
        0x50DF00,
        0x680D00,
        0x707F00,
        0x880700,
        0x59E119FE,
        0x45E78FE,
        0x45E78FE,
        0xAC9FFFE,
        0x45F86FE,
        0x45E78FE,
        0x45F86FE,
        0x976A9FE,
        0x59E2E5FE,
        0x152DFFFE,
        0x1B9E78FE,
        0x152DFFFE,
        0x59E2E5FE,
        0xFAAE07E,
        0x1B9F86FE,
        0x1B9F86FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E119FE,
        0x59E2E5FE,
        0x59E1FFFE,
        0x59E2E5FE,
        0x59E2E5FE,
        0x59E1FFFE,
        0x59E1FFFE,
        0x59E2E5FE,
        0x59E1FFFE,
        0x59E1FFFE,
        0x59E2E5FE,
        0x82B2B,
        0x58FDFD,
        0x10AA00,
        0x1CDD00,
        0x302200,
        0x342F00,
        0x952DFFFE,
        0x9B9D86FE,
        0x952DFFFE,
        0x952DFFFE,
        0x9B9D86FE,
        0x2FFC8C7E,
        0x3335597E,
        0x94DFF37E,
        0x9B1F11FE,
        0x3A61597E,
        0x3335597E,
        0x3335597E,
        0x47FEB9FE,
        0x47FEB9FE,
        0x4C3773FE,
        0x4C3773FE,
        0x47FEB9FE,
        0x1CC000,
        0x203300,
        0x2CFF00,
        0x483F00,
        0x5CFF00,
        0x780200,
        0x780300,
        0x3A61597E,
        0x4C3773FE,
        0x3335597E,
        0x3335597E,
        0x3A61597E,
        0x3A61597E,
        0x3335597E,
        0x2CC6A67E,
        0x2FFDFFFE,
        0x2FFDFFFE,
        0x3A61597E,
        0x3EF1F6FE,
        0x3335597E,
        0x38522C7E,
        0x4C3773FE,
        0x4C3773FE,
        0x4C3773FE,
        0x4C3773FE,
        0x4C3773FE,
        0x4C3773FE,
        0x4C3773FE,
        0x59E2E5FE,
        0x4C3773FE,
        0x530B2C7E,
        0x530B2C7E,
        0x530B2C7E,
        0x4C3773FE,
        0x530B2C7E,
        0x4C3773FE,
        0x4C3773FE,
        0x530B2C7E,
        0x45757,
        0x14EA00,
        0x241500,
        0x2C7700,
        0x401F00,
        0x500100,
        0x1B9E78FE,
        0x1B9E78FE,
        0x1B9F86FE,
        0x1B9F86FE,
        0x1B9F86FE,
        0x1B9E78FE,
        0x1B9E78FE,
        0x1B9F86FE,
        0x2FFB73FE,
        0x1B9E78FE,
        0x2CC6A67E,
        0x366BAFFE,
        0x1B9E78FE,
        0x37529AFE,
        0x59E2E5FE,
        0x59E2E5FE,
        0x59E2E5FE,
        0x59E2E5FE,
        0x59E2E5FE,
        0x59E2E5FE};
#endif
}

bool Octree::raymarch(glm::vec3 &ro,
                      glm::vec3 &rd,
                      glm::vec3 &r_hit,
                      uint32_t &r_normal,
                      int &r_idx)
{
    glm::vec3 rayOrigin = ro;
    glm::vec3 rayDirection = rd;

    const float rayScale = 0.0f;
    const int MaxScale = 23; // Maximum scale (number of float mantissa bits).
    struct StackEntry
    {
        uint64_t offset;
        float maxT;
    };
    StackEntry rayStack[MaxScale + 1];

    float ox = ro.x, oy = ro.y, oz = ro.z;
    float dx = rd.x, dy = rd.y, dz = rd.z;

    if (std::fabs(dx) < 1e-4f)
        dx = 1e-4f;
    if (std::fabs(dy) < 1e-4f)
        dy = 1e-4f;
    if (std::fabs(dz) < 1e-4f)
        dz = 1e-4f;

    float dTx = 1.0f / -std::fabs(dx);
    float dTy = 1.0f / -std::fabs(dy);
    float dTz = 1.0f / -std::fabs(dz);

    float bTx = dTx * ox;
    float bTy = dTy * oy;
    float bTz = dTz * oz;

    uint8_t octantMask = 7;
    if (dx > 0.0f)
        octantMask ^= 1, bTx = 3.0f * dTx - bTx;
    if (dy > 0.0f)
        octantMask ^= 2, bTy = 3.0f * dTy - bTy;
    if (dz > 0.0f)
        octantMask ^= 4, bTz = 3.0f * dTz - bTz;

    float minT = std::max(2.0f * dTx - bTx, std::max(2.0f * dTy - bTy, 2.0f * dTz - bTz));
    float maxT = std::min(dTx - bTx, std::min(dTy - bTy, dTz - bTz));
    minT = std::max(minT, 0.0f);

    uint32_t current = 0;
    uint64_t parent = 0;
    int idx = 0;
    float posX = 1.0f;
    float posY = 1.0f;
    float posZ = 1.0f;
    int scale = MaxScale - 1;

    float scaleExp2 = 0.5f;

    if (1.5f * dTx - bTx > minT)
        idx ^= 1, posX = 1.5f;
    if (1.5f * dTy - bTy > minT)
        idx ^= 2, posY = 1.5f;
    if (1.5f * dTz - bTz > minT)
        idx ^= 4, posZ = 1.5f;

    while (scale < MaxScale)
    {
        if (current == 0)
        {
            // printf("Parent: %u\n", parent);
            if(parent >= _treeSize) { return false; }
            current = _tree[parent];
        }

        float cornerTX = posX * dTx - bTx;
        float cornerTY = posY * dTy - bTy;
        float cornerTZ = posZ * dTz - bTz;
        float maxTC = std::min(cornerTX, std::min(cornerTY, cornerTZ));

        int childShift = idx ^ octantMask;
        uint32_t childMasks = current << childShift;

        if ((childMasks & 0x8000) && minT <= maxT)
        {
            if (maxTC * rayScale >= scaleExp2)
            {
                // Testing return value
                r_hit.x = std::min(std::max(ro.x + minT * rd.x, ro.x + 1e-4f), ro.x + scaleExp2 - 1e-4f);
                r_hit.y = std::min(std::max(ro.y + minT * rd.y, ro.y + 1e-4f), ro.y + scaleExp2 - 1e-4f);
                r_hit.z = std::min(std::max(ro.z + minT * rd.z, ro.z + 1e-4f), ro.z + scaleExp2 - 1e-4f);

                r_idx = idx;
                return true;
            }

            float maxTV = std::min(maxT, maxTC);
            float half = scaleExp2 * 0.5f;
            float centerTX = half * dTx + cornerTX;
            float centerTY = half * dTy + cornerTY;
            float centerTZ = half * dTz + cornerTZ;

            if (minT <= maxTV)
            {
                uint64_t childOffset = current >> 18;
                if (current & 0x20000)
                    childOffset = (childOffset << 32) | uint64_t(_tree[parent + 1]);

                if (!(childMasks & 0x80))
                {
                    break;
                }

                rayStack[scale].offset = parent;
                rayStack[scale].maxT = maxT;

                uint32_t siblingCount = BitCount[childMasks & 127];
                parent += childOffset + siblingCount;
                // printf("Sibling Count: %u\n", siblingCount);
                if (current & 0x10000)
                    parent += siblingCount;

                idx = 0;
                scale--;
                scaleExp2 = half;

                if (centerTX > minT)
                    idx ^= 1, posX += scaleExp2;
                if (centerTY > minT)
                    idx ^= 2, posY += scaleExp2;
                if (centerTZ > minT)
                    idx ^= 4, posZ += scaleExp2;

                maxT = maxTV;
                current = 0;

                continue;
            }
        }

        int stepMask = 0;
        if (cornerTX <= maxTC)
            stepMask ^= 1, posX -= scaleExp2;
        if (cornerTY <= maxTC)
            stepMask ^= 2, posY -= scaleExp2;
        if (cornerTZ <= maxTC)
            stepMask ^= 4, posZ -= scaleExp2;

        minT = maxTC;
        idx ^= stepMask;

        if ((idx & stepMask) != 0)
        {
            int differingBits = 0;
            if (stepMask & 1)
                differingBits |= floatBitsToUint(posX) ^ floatBitsToUint(posX + scaleExp2);
            if (stepMask & 2)
                differingBits |= floatBitsToUint(posY) ^ floatBitsToUint(posY + scaleExp2);
            if (stepMask & 4)
                differingBits |= floatBitsToUint(posZ) ^ floatBitsToUint(posZ + scaleExp2);
            scale = (floatBitsToUint((float)differingBits) >> 23) - 127;
            scaleExp2 = uintBitsToFloat((scale - MaxScale + 127) << 23);

            parent = rayStack[scale].offset;
            maxT = rayStack[scale].maxT;

            int shX = floatBitsToUint(posX) >> scale;
            int shY = floatBitsToUint(posY) >> scale;
            int shZ = floatBitsToUint(posZ) >> scale;
            posX = uintBitsToFloat(shX << scale);
            posY = uintBitsToFloat(shY << scale);
            posZ = uintBitsToFloat(shZ << scale);
            idx = (shX & 1) | ((shY & 1) << 1) | ((shZ & 1) << 2);

            current = 0;
        }
    }

    if (scale >= MaxScale)
        return false;

    // Testing return value
    r_hit.x = std::min(std::max(ro.x + minT * rd.x, ro.x + 1e-4f), ro.x + scaleExp2 - 1e-4f);
    r_hit.y = std::min(std::max(ro.y + minT * rd.y, ro.y + 1e-4f), ro.y + scaleExp2 - 1e-4f);
    r_hit.z = std::min(std::max(ro.z + minT * rd.z, ro.z + 1e-4f), ro.z + scaleExp2 - 1e-4f);

    r_idx = idx;
    return true;
}