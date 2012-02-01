#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "PacketInjector.h"

// метод будет вызываться самое позднее раз в 250 миллисекунд
void UserAI::UAI_ProcessFollow( class UserAI *cls )
{
	if( !cls->folAsiCfg.m_follow_enable ) return;
	if( cls->folAsiCfg.m_follow_dist < 0 ) return;
	// check time
	unsigned int t_now = GetTickCount();
	if( (t_now - cls->m_last_follow_time_msec) < 1000 ) return; // follow only once per second
	// check party
	if( cls->folAsiCfg.m_follow_only_in_party && cls->party.getCount() < 1 ) return;
	// get follow target oid
	L2Player *plFollow = NULL;
	if( cls->m_follow_oid == 0 )
	{
		log_error( LOG_USERAI, "Follow enabled after [%S], searching for its objectID...\n",
			cls->folAsiCfg.m_follow_name );
		// придется пройтись по всему массиву чаров, сравнивя по имени
		// это небыстрая операция, и вдобавок придется держать блокировку
		// на всем массиве чаров, чтобы никакой другой поток его не изменил
		// пока мы по нему гуляем
		unsigned int i = 0, cnt = CharArray_GetCount(), nc = 0;
		CharArray_Lock();
		for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
		{
			if( chars_array[i] )
			{
				if( _wcsicmp( chars_array[i]->charName, cls->folAsiCfg.m_follow_name ) == 0 )
				{
					plFollow = chars_array[i];
					break;
				}
				nc++;
			}
			if( nc >= cnt ) break;
		}
		CharArray_Unlock(); // снимем наконец-то блокировку
		if( !plFollow ) // объекта цели follow нет в зоне видимости
		{
			//log_error( LOG_USERAI, ":( cannot find follow target oid - not in range?\n" );
			return;
		}
		// уря, нашли
		cls->m_follow_oid = plFollow->objectID; // Запомним objectID чтоб больше не искать
		UAI_Check_Distance_And_Follow( cls, plFollow );
		return;
	}
	else // мы уже знаем oid цели преследования
	{
		int idx = -1;
		L2OBJECT_TYPE objType = L2OT_NONE;
		// а вот это уже не полный перебор, о поиск по objectID в бинарном дереве, довольно быстрый
		if( WorldObjectTree_GetInfoByObjectID( cls->m_follow_oid, &objType, &idx ) )
		{
			if( objType == L2OT_PC )
			{
				if( idx >= 0 )
					UAI_Check_Distance_And_Follow( cls, chars_array[idx] );
			}
			else log_error( LOG_ERROR, "UAI Follow: follow target objType != L2OT_PC (%d)\n", (int)objType );
		}
		//else log_error( LOG_USERAI, "Follow: [%S] is not in range\n", cls->folAsiCfg.m_follow_name );
	}
}

void UserAI::UAI_Check_Distance_And_Follow( class UserAI *cls, L2Player *followTarget )
{
	float dx = (float)( followTarget->x - cls->usr.x );
	float dy = (float)( followTarget->y - cls->usr.y );
	float dz = (float)( followTarget->z - cls->usr.z );
	float dist = sqrtf( dx*dx + dy*dy + dz*dz );
	if( dist > 10000.0f ) // как-то уж слишком далековато, не ошибка ли?
	{
		log_error( LOG_USERAI, "!!! Dist to target is %0.2f!!! WTF???\n", dist );
		return;
	}
	if( (int)dist > cls->folAsiCfg.m_follow_dist )
	{
		cls->m_last_follow_time_msec = GetTickCount();
		PGen_MoveBackwardToLocation( followTarget->x, followTarget->y, followTarget->z,
			cls->usr.x, cls->usr.y, cls->usr.z );
		//log_error( LOG_USERAI, "Following... dist = %0.2f\n", dist );
	}
}

void UserAI::followDisable()
{
	this->folAsiCfg.m_follow_enable = 0;
	this->m_follow_oid = 0;
}

void UserAI::assistDisable()
{
	this->folAsiCfg.m_assist_enable = 0;
	this->m_assist_oid = 0;
}



void UserAI::UAI_ProcessAssist( class UserAI *cls )
{
	if( !cls->folAsiCfg.m_assist_enable ) return;
	if( cls->folAsiCfg.m_assist_only_in_party && cls->party.getCount() < 1 ) return;
	// найдем oID МА
	L2Player *plAssist = NULL;
	if( cls->m_assist_oid == 0 )
	{
		log_error( LOG_USERAI, "Assist enabled by [%S], searching for its objectID...\n",
			cls->folAsiCfg.m_assist_name );
		// придется пройтись по всему массиву чаров, сравнивя по имени
		// это небыстрая операция, и вдобавок придется держать блокировку
		// на всем массиве чаров, чтобы никакой другой поток его не изменил
		// пока мы по нему гуляем
		unsigned int i = 0, cnt = CharArray_GetCount(), nc = 0;
		CharArray_Lock();
		for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
		{
			if( chars_array[i] )
			{
				if( _wcsicmp( chars_array[i]->charName, cls->folAsiCfg.m_assist_name ) == 0 )
				{
					plAssist = chars_array[i];
					break;
				}
				nc++;
			}
			if( nc >= cnt ) break;
		}
		CharArray_Unlock(); // снимем наконец-то блокировку
		if( !plAssist ) // объекта цели assist нет в зоне видимости
		{
			//log_error( LOG_USERAI, ":( cannot find assist target oid - not in range?\n" );
			return;
		}
		// уря, нашли
		log_error( LOG_USERAI, "Found assist oID: %u\n", plAssist->objectID );
		cls->m_assist_oid = plAssist->objectID; // Запомним objectID чтоб больше не искать
		UAI_Check_Target_By_Assist( cls, plAssist );
	}
	else // мы уже знаем oid МА
	{
		int idx = -1;
		L2OBJECT_TYPE objType = L2OT_NONE;
		// а вот это уже не полный перебор, о поиск по objectID в бинарном дереве, довольно быстрый
		if( WorldObjectTree_GetInfoByObjectID( cls->m_assist_oid, &objType, &idx ) )
		{
			if( objType == L2OT_PC )
			{
				if( idx >= 0 )
					UAI_Check_Target_By_Assist( cls, chars_array[idx] );
			}
			else log_error( LOG_ERROR, "UAI Assist: MA objType != L2OT_PC (%d)\n", (int)objType );
		}
	}
}

void UserAI::UAI_Check_Target_By_Assist( class UserAI *cls, L2Player *assistBy )
{
	unsigned int now_time = GetTickCount();
	if( (now_time - cls->m_last_assist_time_msec) < 500 ) return; // assist once in 500 msec
	if( assistBy->targetObjectID != 0 ) // у МА есть кто-то на таргете
	{
		// проверим кто на таргете у МА...
		unsigned int targetObjectID = assistBy->targetObjectID;
		int idx = -1;
		L2OBJECT_TYPE objType = L2OT_NONE;
		bool target_valid = false;
		L2Player *target = NULL;
		if( WorldObjectTree_GetInfoByObjectID( targetObjectID, &objType, &idx ) )
		{
			if( objType == L2OT_PC  &&  idx >= 0 )
			{
				target = chars_array[idx];
				if( !target ) return;
				target_valid = true;
				if( cls->folAsiCfg.m_assist_dont_ally )
				{
					if( cls->party.getCount() > 0 && cls->party.isInParty( targetObjectID, NULL ) ) target_valid = false;
					if( target->clanID == cls->usr.clanID ) target_valid = false;
					if( target->allyID == cls->usr.allyID ) target_valid = false;
				}
				//if( target_valid )
				//	log_error( LOG_USERAI, "Assist: MA targeted %u [%S] (valid)\n", targetObjectID, target->charName );
				//else
				//	log_error( LOG_USERAI, "Assist: MA targeted %u [%S] (invalid)\n", targetObjectID, target->charName );
			}
			//else if( objType == L2OT_NPC )
			//	log_error( LOG_USERAI, "Assist: MA targeted NPC (invalid)\n" );
		}
		if( !target_valid ) return;
		if( cls->usr.targetObjectID != targetObjectID ) // наш таргет != таргету МА
		{
			log_error( LOG_USERAI, "ReAssist: MA targeted %u [%S] (valid)\n", targetObjectID, target->charName );
			PGen_Action( targetObjectID, cls->usr.x, cls->usr.y, cls->usr.z, 0 );
			cls->m_last_assist_time_msec = now_time; // remember Action time
		}
	}
}
