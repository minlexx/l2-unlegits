#ifndef H_UTILS
#define H_UTILS

void Utils_process_SystemMessage( void *l2_game_packet, wchar_t *out );
void Utils_replace_in_SM_template_S( wchar_t *sm_template, int iPos, const wchar_t *str );

#endif
