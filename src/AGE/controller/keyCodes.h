#ifndef __KEY_CODES_H_
#define __KEY_CODES_H_
#include <string>
#include <array>
#include <algorithm>

namespace KEYCODE{
constexpr const int  AGE_KEY_CODE_YES  = 0400;  /* A wchar_t contains a key code */
constexpr const int  AGE_KEY_MIN = 0401;		/* Minimum curses key */
constexpr const int  AGE_KEY_BREAK	= 0401;		/* Break key (unreliable) */
constexpr const int  AGE_KEY_SRESET	= 0530;		/* Soft (partial) reset (unreliable) */
constexpr const int  AGE_KEY_RESET	= 0531;		/* Reset or hard reset (unreliable) */
constexpr const int  AGE_KEY_DOWN	= 0402;		/* down-arrow key */
constexpr const int  AGE_KEY_UP		= 0403;		/* up-arrow key */
constexpr const int  AGE_KEY_LEFT	= 0404;		/* left-arrow key */
constexpr const int  AGE_KEY_RIGHT	= 0405;		/* right-arrow key */
constexpr const int  AGE_KEY_HOME	= 0406;		/* home key */
constexpr const int  AGE_KEY_BACKSPACE	= 0407;		/* backspace key */
constexpr const int  AGE_KEY_F0 =	0410;		/* Function keys.  Space for 64 */
constexpr const int  AGE_KEY_F(int n){	
    return (AGE_KEY_F0+(n));	
}
constexpr const int  AGE_KEY_DL		= 0510;		/* delete-line key */
constexpr const int  AGE_KEY_IL		= 0511;		/* insert-line key */
constexpr const int  AGE_KEY_DC		= 0512;		/* delete-character key */
constexpr const int  AGE_KEY_IC		= 0513;		/* insert-character key */
constexpr const int  AGE_KEY_EIC		= 0514;		/* sent by rmir or smir in insert mode */
constexpr const int  AGE_KEY_CLEAR	= 0515;		/* clear-screen or erase key */
constexpr const int  AGE_KEY_EOS		= 0516;		/* clear-to-end-of-screen key */
constexpr const int  AGE_KEY_EOL		= 0517;		/* clear-to-end-of-line key */
constexpr const int  AGE_KEY_SF		= 0520;		/* scroll-forward key */
constexpr const int  AGE_KEY_SR		= 0521;		/* scroll-backward key */
constexpr const int  AGE_KEY_NPAGE	= 0522;		/* next-page key */
constexpr const int  AGE_KEY_PPAGE	= 0523;		/* previous-page key */
constexpr const int  AGE_KEY_STAB	= 0524;		/* set-tab key */
constexpr const int  AGE_KEY_CTAB	= 0525;		/* clear-tab key */
constexpr const int  AGE_KEY_CATAB	= 0526;		/* clear-all-tabs key */
constexpr const int  AGE_KEY_ENTER	= 0527;		/* enter/send key */
constexpr const int  AGE_KEY_PRINT	= 0532;		/* print key */
constexpr const int  AGE_KEY_LL		= 0533;		/* lower-left key (home down) */
constexpr const int  AGE_KEY_A1		= 0534;		/* upper left of keypad */
constexpr const int  AGE_KEY_A3		= 0535;		/* upper right of keypad */
constexpr const int  AGE_KEY_B2		= 0536;		/* center of keypad */
constexpr const int  AGE_KEY_C1		= 0537;		/* lower left of keypad */
constexpr const int  AGE_KEY_C3		= 0540;		/* lower right of keypad */
constexpr const int  AGE_KEY_BTAB	= 0541;		/* back-tab key */
constexpr const int  AGE_KEY_BEG		= 0542;		/* begin key */
constexpr const int  AGE_KEY_CANCEL	= 0543;		/* cancel key */
constexpr const int  AGE_KEY_CLOSE	= 0544;		/* close key */
constexpr const int  AGE_KEY_COMMAND	= 0545;		/* command key */
constexpr const int  AGE_KEY_COPY	= 0546;		/* copy key */
constexpr const int  AGE_KEY_CREATE	= 0547;		/* create key */
constexpr const int  AGE_KEY_END		= 0550;		/* end key */
constexpr const int  AGE_KEY_EXIT	= 0551;		/* exit key */
constexpr const int  AGE_KEY_FIND	= 0552;		/* find key */
constexpr const int  AGE_KEY_HELP	= 0553;		/* help key */
constexpr const int  AGE_KEY_MARK	= 0554;		/* mark key */
constexpr const int  AGE_KEY_MESSAGE	= 0555;		/* message key */
constexpr const int  AGE_KEY_MOVE	= 0556;		/* move key */
constexpr const int  AGE_KEY_NEXT	= 0557;		/* next key */
constexpr const int  AGE_KEY_OPEN	= 0560;		/* open key */
constexpr const int  AGE_KEY_OPTIONS	= 0561;		/* options key */
constexpr const int  AGE_KEY_PREVIOUS	= 0562;		/* previous key */
constexpr const int  AGE_KEY_REDO	= 0563;		/* redo key */
constexpr const int  AGE_KEY_REFERENCE	= 0564;		/* reference key */
constexpr const int  AGE_KEY_REFRESH	= 0565;		/* refresh key */
constexpr const int  AGE_KEY_REPLACE	= 0566;		/* replace key */
constexpr const int  AGE_KEY_RESTART	= 0567;		/* restart key */
constexpr const int  AGE_KEY_RESUME	= 0570;		/* resume key */
constexpr const int  AGE_KEY_SAVE	= 0571;		/* save key */
constexpr const int  AGE_KEY_SBEG	= 0572;		/* shifted begin key */
constexpr const int  AGE_KEY_SCANCEL	= 0573;		/* shifted cancel key */
constexpr const int  AGE_KEY_SCOMMAND	= 0574;		/* shifted command key */
constexpr const int  AGE_KEY_SCOPY	= 0575;		/* shifted copy key */
constexpr const int  AGE_KEY_SCREATE	= 0576;		/* shifted create key */
constexpr const int  AGE_KEY_SDC		= 0577;		/* shifted delete-character key */
constexpr const int  AGE_KEY_SDL		= 0600;		/* shifted delete-line key */
constexpr const int  AGE_KEY_SELECT	= 0601;		/* select key */
constexpr const int  AGE_KEY_SEND	= 0602;		/* shifted end key */
constexpr const int  AGE_KEY_SEOL	= 0603;		/* shifted clear-to-end-of-line key */
constexpr const int  AGE_KEY_SEXIT	= 0604;		/* shifted exit key */
constexpr const int  AGE_KEY_SFIND	= 0605;		/* shifted find key */
constexpr const int  AGE_KEY_SHELP	= 0606;		/* shifted help key */
constexpr const int  AGE_KEY_SHOME	= 0607;		/* shifted home key */
constexpr const int  AGE_KEY_SIC		= 0610;		/* shifted insert-character key */
constexpr const int  AGE_KEY_SLEFT	= 0611;		/* shifted left-arrow key */
constexpr const int  AGE_KEY_SMESSAGE	= 0612;		/* shifted message key */
constexpr const int  AGE_KEY_SMOVE	= 0613;		/* shifted move key */
constexpr const int  AGE_KEY_SNEXT	= 0614;		/* shifted next key */
constexpr const int  AGE_KEY_SOPTIONS	= 0615;		/* shifted options key */
constexpr const int  AGE_KEY_SPREVIOUS	= 0616;		/* shifted previous key */
constexpr const int  AGE_KEY_SPRINT	= 0617;		/* shifted print key */
constexpr const int  AGE_KEY_SREDO	= 0620;		/* shifted redo key */
constexpr const int  AGE_KEY_SREPLACE	= 0621;		/* shifted replace key */
constexpr const int  AGE_KEY_SRIGHT	= 0622;		/* shifted right-arrow key */
constexpr const int  AGE_KEY_SRSUME	= 0623;		/* shifted resume key */
constexpr const int  AGE_KEY_SSAVE	= 0624;		/* shifted save key */
constexpr const int  AGE_KEY_SSUSPEND	= 0625;		/* shifted suspend key */
constexpr const int  AGE_KEY_SUNDO	= 0626;		/* shifted undo key */
constexpr const int  AGE_KEY_SUSPEND	= 0627;		/* suspend key */
constexpr const int  AGE_KEY_UNDO	= 0630;		/* undo key */
constexpr const int  AGE_KEY_MOUSE	= 0631;		/* Mouse event has occurred */
constexpr const int  AGE_KEY_RESIZE	= 0632;		/* Terminal resize event */
constexpr const int  AGE_KEY_EVENT	= 0633;		/* We were interrupted by an event */
constexpr const int  AGE_KEY_MAX		= 0777;		/* Maximum key value is 0633 */
}



namespace AGE{

template<typename Key, typename Value, size_t Size>
struct CMap{
    std::array<std::pair<Key, Value>, Size> arr;
    constexpr Value at(const Key &key) const {
        const auto itt = std::find_if(arr.begin(), arr.end(),
            [&key](const auto &v){return v.first == key;});
        if(itt != arr.end()){
            return itt->second; 
        }
        else{
            throw std::range_error("Not found");
        }
    }
};

static std::array<std::pair<int, std::string>, 95> keyarr{{
{0400, "AGE_KEY_CODE_YES"},
{0401, "AGE_KEY_MIN"},
{0401, "AGE_KEY_BREAK"},
{0530, "AGE_KEY_SRESET"},
{0531, "AGE_KEY_RESET"},
{0402, "AGE_KEY_DOWN"},
{0403, "AGE_KEY_UP"},
{0404, "AGE_KEY_LEFT"},
{0405, "AGE_KEY_RIGHT"},
{0406, "AGE_KEY_HOME"},
{0407, "AGE_KEY_BACKSPACE"},
{0410, "AGE_KEY_F0"},
{0510, "AGE_KEY_DL"},
{0511, "AGE_KEY_IL"},
{0512, "AGE_KEY_DC"},
{0513, "AGE_KEY_IC"},
{0514, "AGE_KEY_EIC "},
{0515, "AGE_KEY_CLEAR"},
{0516, "AGE_KEY_EOS"},
{0517, "AGE_KEY_EOL"},
{0520, "AGE_KEY_SF"},
{0521, "AGE_KEY_SR"},
{0522, "AGE_KEY_NPAGE"},
{0523, "AGE_KEY_PPAGE"},
{0524, "AGE_KEY_STAB"},
{0525, "AGE_KEY_CTAB"},
{0526, "AGE_KEY_CATAB"},
{0527, "AGE_KEY_ENTER"},
{0532, "AGE_KEY_PRINT"},
{0533, "AGE_KEY_LL"},
{0534, "AGE_KEY_A1"},
{0535, "AGE_KEY_A3"},
{0536, "AGE_KEY_B2"},
{0537, "AGE_KEY_C1"},
{0540, "AGE_KEY_C3"},
{0541, "AGE_KEY_BTAB"},
{0542, "AGE_KEY_BEG"},
{0543, "AGE_KEY_CANCEL"},
{0544, "AGE_KEY_CLOSE"},
{0545, "AGE_KEY_COMMAND"},
{0546, "AGE_KEY_COPY"},
{0547, "AGE_KEY_CREATE"},
{0550, "AGE_KEY_END"},
{0551, "AGE_KEY_EXIT"},
{0552, "AGE_KEY_FIND"},
{0553, "AGE_KEY_HELP"},
{0554, "AGE_KEY_MARK"},
{0555, "AGE_KEY_MESSAGE"},
{0556, "AGE_KEY_MOVE"},
{0557, "AGE_KEY_NEXT"},
{0560, "AGE_KEY_OPEN"},
{0561, "AGE_KEY_OPTIONS"},
{0562, "AGE_KEY_PREVIOUS"},
{0563, "AGE_KEY_REDO"},
{0564, "AGE_KEY_REFERENCE"},
{0565, "AGE_KEY_REFRESH"},
{0566, "AGE_KEY_REPLACE"},
{0567, "AGE_KEY_RESTART"},
{0570, "AGE_KEY_RESUME"},
{0571, "AGE_KEY_SAVE"},
{0572, "AGE_KEY_SBEG"},
{0573, "AGE_KEY_SCANCEL"},
{0574, "AGE_KEY_SCOMMAND"},
{0575, "AGE_KEY_SCOPY"},
{0576, "AGE_KEY_SCREATE"},
{0577, "AGE_KEY_SDC"},
{0600, "AGE_KEY_SDL"},
{0601, "AGE_KEY_SELECT"},
{0602, "AGE_KEY_SEND"},
{0603, "AGE_KEY_SEOL"},
{0604, "AGE_KEY_SEXIT"},
{0605, "AGE_KEY_SFIND"},
{0606, "AGE_KEY_SHELP"},
{0607, "AGE_KEY_SHOME"},
{0610, "AGE_KEY_SIC"},
{0611, "AGE_KEY_SLEFT"},
{0612, "AGE_KEY_SMESSAGE"},
{0613, "AGE_KEY_SMOVE"},
{0614, "AGE_KEY_SNEXT"},
{0615, "AGE_KEY_SOPTIONS"},
{0616, "AGE_KEY_SPREVIOUS"},
{0617, "AGE_KEY_SPRINT"},
{0620, "AGE_KEY_SREDO"},
{0621, "AGE_KEY_SREPLACE"},
{0622, "AGE_KEY_SRIGHT"},
{0623, "AGE_KEY_SRSUME"},
{0624, "AGE_KEY_SSAVE"},
{0625, "AGE_KEY_SSUSPEND"},
{0626, "AGE_KEY_SUNDO"},
{0627, "AGE_KEY_SUSPEND"},
{0630, "AGE_KEY_UNDO"},
{0631, "AGE_KEY_MOUSE"},
{0632, "AGE_KEY_RESIZE"},
{0633, "AGE_KEY_EVENT "},
{0777, "AGE_KEY_MAX"}}
};

}

#endif