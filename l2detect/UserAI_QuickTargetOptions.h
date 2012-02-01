#pragma once

class UserAI_QuickTargetOptions
{
public:
	UserAI_QuickTargetOptions();
public:
	int  m_use_pri;
	int  m_target_dead;
	int  m_target_party;
	int  m_target_clan;
	int  m_target_ally;
	int  m_target_cw_2_side;
	int  m_target_cw_1_side;
	int  m_target_neutral_pk;
	int  m_target_neutral_flagged;
};

/*
Disilon (18:12:33 23/01/2010)
т.е. в конфигах нужна закладка например PC, там галочки:
1)показывать двусторонние клан вары
2)показывать односторонки
3)показывать сопартийцев
4)показывать сокланов
5)показывать соаллийцев
6)показывать нейтралов флагнутых
7)показывать нейтралов пк
8)показывать мертвых
9)использовать приоритеты
*/
