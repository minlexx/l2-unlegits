-- script globals
script_name = "AI: ";
sys_isResting = false;

l2h_print( "Sys AI script started\n" );

-- sys config vars
botting_combat_enable = 0;
-- self heal
self_heal_sit_hp_enable = 0;
self_heal_sit_mp_enable = 0;
self_heal_sit_hp = 0;
self_heal_sit_mp = 0;
self_heal_stand_hp_enable = 0;
self_heal_stand_mp_enable = 0;
self_heal_stand_hp = 0;
self_heal_stand_mp = 0;
self_heal_use_item_nConds = 0;
self_heal_use_skill_nConds = 0;
-- party
party_loot = 0;
--Iparty_auto_accept_enable=1 -- used by system
--Sparty_auto_accept_names=EPLLI -- used by system
party_auto_invite_enable = 0;
party_auto_invite_names = "";
party_assist_enable = 0;
follow_enable = 0;
follow_name = "";
follow_dist = 0;

function cfg_load_self_heal()
	self_heal_sit_hp_enable = l2ccfg_getInt( "self_heal_sit_hp_enable" );
	self_heal_sit_mp_enable = l2ccfg_getInt( "self_heal_sit_mp_enable" );
	self_heal_sit_hp = l2ccfg_getInt( "self_heal_sit_hp" );
	self_heal_sit_mp = l2ccfg_getInt( "self_heal_sit_mp" );
	self_heal_stand_hp_enable = l2ccfg_getInt( "self_heal_stand_hp_enable" );
	self_heal_stand_mp_enable = l2ccfg_getInt( "self_heal_stand_mp_enable" );
	self_heal_stand_hp = l2ccfg_getInt( "self_heal_stand_hp" );
	self_heal_stand_mp = l2ccfg_getInt( "self_heal_stand_mp" );
	botting_combat_enable = l2c_isCombatEnabled();
	self_heal_use_item_nConds = l2ccfg_getInt( "self_heal_use_item_nConds" );
	self_heal_use_skill_nConds = l2ccfg_getInt( "self_heal_use_skill_nConds" );
end

function cfg_load_party()
	party_loot = l2ccfg_getInt( "party_loot" );
	party_auto_invite_enable = l2ccfg_getInt( "party_auto_invite_enable" );
	party_auto_invite_names = l2ccfg_getStr( "party_auto_invite_names" );
	party_assist_enable = l2ccfg_getInt( "party_assist_enable" );
	follow_enable = l2ccfg_getInt( "follow_enable" );
	follow_name = l2ccfg_getStr( "follow_name" );
	follow_dist = l2ccfg_getInt( "follow_dist" );
end


--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------

function ensurestand()
	local isSitting, isRunnig = l2c_getSitRun();
	if isSitting then
		l2c_sitStand();
		l2h_delay( 1000 );
	end
end

function ensuresit()
	local isSitting, isRunnig = l2c_getSitRun();
	if not isSitting then
		l2c_sitStand();
		l2h_delay( 1000 );
	end
end

function ensurerun()
	local isSitting, isRunnig = l2c_getSitRun()
	if not isRunnig then
		l2c_runWalk();
		l2h_delay( 50 );
	end
end

function ensurewalk()
	local isSitting, isRunnig = l2c_getSitRun()
	if isRunnig then
		l2c_runWalk();
		l2h_delay( 50 );
	end
end

-- вернет true, если баф skillID присутсвует в массиве have_buffs
function isBuffPresent( skillID, have_buffs )
	if not have_buffs then return end
	local i = 0;
	for i=1, #have_buffs do
		if have_buffs[i].skillID == skillID then return true end
	end
	return false
end

function wait_end_of_cast()
	while l2c_isCastingNow() do l2h_delay( 50 ) end
end

-- функция ждет когда реюз скилла будет закончен
function wait_skill_reuse( skillID )
	local msecLeft = l2c_getSkillReuseLeft( skillID );
	if msecLeft <= 0 then return end
	l2h_delay( msecLeft )
end

-- true: скилл готов к реюзу, false - ещё идет откат
function isSkillReadyForReuse( skillID )
	local msecLeft = l2c_getSkillReuseLeft( skillID );
	if msecLeft <= 0 then return true end
	return false
end

-- пытается затаргетить objectID в течение timeoutSecs секунд
-- вернет true, если таргет удался
function myTargetObjectTimeout( objectID, timeoutSecs )
	local targetStartTime = l2h_time();
	-- может отбъект уже на таргете?
	local targetObjectId = l2c_getTarget();
	if targetObjectId == objectID then return end -- ага, уже на таргете
	-- иначе, таргетим его
	l2c_action( objectID );
	local timePassed = 0;
	repeat
		targetObjectId = l2c_getTarget();
		l2h_delay( 100 );
		if sys_should_exit() then return end
		local curTime = l2h_time();
		timePassed = curTime - targetStartTime;
	until (timePassed > timeoutSecs) or (targetObjectId == objectID)
	if( targetObjectId ~= objectID ) then
		l2h_print( script_name, " ERROR: cannot target [", objectID, "]\n" );
		return false
	end
	return true
end

-- использует скилл и делает задержку 100мс ля уверенности что каст скилла начался
-- чиста для удобства. величина задержки зависит от лагов канала до сервера
function myUseSkill( skillID )
	l2c_useSkill( skillID )
	l2h_delay( 150 )
end

-- функция
-- следовать за чаром [name] на дистанции [distance]
-- если заданный чар уже находится в пределах [distance], follow не существляется
-- randomness - число, в пределах которого следование отклоняется от цели. рекомендуется 1-100
function follow( name, distance, randomness )
	-- получим objectID чара
	local objectID = l2c_getCharObjectIdByName( name )
	-- а есть ли такой чар вообще?
	if objectID == nil then
		--l2h_print( script_name, " Character [", name, "] is not visible!\n" );
		l2h_delay( 200 );
		return
	end
	-- случайное смещение не может быть больше дистанции следования!
	-- должно быть хотя б в 2 раза меньше
	if randomness >= distance then randomness = distance/2; end
	local x, y, z -- наши координаты будут тут
	local c       -- инфо о чаре будет тут
	-- а надо ли нам следовать?
	x,y,z = l2c_getPos() -- получим наши координаты
	c = l2c_getObjectInfoByObjectId( objectID ) -- инфо о цели "преследования"
	if not c then return end -- объект ПРОПАЛ в течение последних 25-50 мс :(
	if (math.abs(c.x - x) < distance) and (math.abs(c.y - y) < distance) then
		return -- да мы и так недалеко от цели
	end
	-- цикл следования
	repeat
		if sys_should_exit() then return end -- юзер нажал "Стоп скрипт"
		-- получим инфо о чаре
		c = l2c_getObjectInfoByObjectId( objectID );
		if not c then return end -- объект ПРОПАЛ в течение последних 25-50 мс :(
		-- случайное смещение влево-вправо в заданных пределах
		local x_rand = math.random( (0-randomness), randomness );
		local y_rand = math.random( (0-randomness), randomness );
		l2c_moveTo( c.x + x_rand, c.y + y_rand, c.z ) -- двинемся к нему
		l2h_delay( 300 ) -- подождем 300 ms
		x,y,z = l2c_getPos() -- получим наши координаты
		-- проверим разницу наших координат и координат цели.
		-- если разница больше дистанции следования, цикл следования повторится
	until (math.abs(c.x - x) < distance) and (math.abs(c.y - y) < distance)
end

function isCharInParty( name, party )
	--l2c_getParty() always returns not nil
	if #party < 1 then return false end
	local i = 0
	for i=1, #party do
		if party[i].name == name then return true end
	end
	return false
end

-- таблица (ассоциативный массив, в данном случае), которая хранит последнее время использования итема с индексом ItemID
sys_item_last_use_time_ms = { 0 };

-- заглядывает в таблицу, смотрит время последнего юза, смотрит текущее время, проверяет задержку
-- если итем не юзался (нет в таблице), вернет true..
function isItemReadyForReuse( itemID, reuseDelayMS )
	local cur_time_ms = l2h_timeMsec();
	local last_use_time_ms = cur_time_ms - reuseDelayMS - 1000;
	if sys_item_last_use_time_ms[ itemID ] ~= nil then
		last_use_time_ms = sys_item_last_use_time_ms[ itemID ];
		--l2h_print( "isItemReadyForReuse(): itemID ", itemID, " was used at ", last_use_time_ms, " ms\n" );
	end
	local next_use_time_ms = last_use_time_ms + reuseDelayMS;
	if next_use_time_ms <= cur_time_ms then return true end -- реюз прошел
	return false -- реюз не прошел )
end

-- юзает итем и заносит время его юза в таблицу
function myUseItem( itemID )
	l2c_useItem( itemID );
	sys_item_last_use_time_ms[ itemID ] = l2h_timeMsec();
end

--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------

function general_ai_self_rest_sit_stand()
	local curHp, maxHp, curMp, maxMp = l2c_getHPMPCPWeight();
	local hpp = 100.0 * curHp / maxHp;
	local mpp = 100.0 * curMp / maxMp;
	-- check config
	cfg_load_self_heal();
	local hp_bad = false;
	local mp_bad = false;
	local hp_good = true;
	local mp_good = true;
	if self_heal_sit_hp_enable and (hpp < self_heal_sit_hp) then hp_bad = true; end
	if self_heal_sit_mp_enable and (mpp < self_heal_sit_mp) then mp_bad = true; end
	if self_heal_stand_hp_enable and (hpp < self_heal_stand_hp) then hp_good = false; end
	if self_heal_stand_mp_enable and (mpp < self_heal_stand_mp) then mp_good = false; end
	if not sys_isResting then -- НЕ отдыхаем, а может надо отдохнуть?
		if hp_bad or mp_bad then -- надо
			l2h_print( script_name, " Rest start, HP ", hpp, "%, MP ", mpp, "%, sitting\n" );
			ensuresit();
			sys_isResting = true; -- Отдыхаем
			return
		end
	else -- уже отдыхаем, а может пора встать
		local isSitting, isRunning = l2c_getSitRun();
		if not isSitting then -- а мы уже не сидим! моб нас спалил??
			sys_isResting = false; -- не отдыхаем!
			l2h_print( script_name, " Rest interrupted (stand)?\n" );
			--l2h_soundAlert();
			return
		end
		if hp_good and mp_good then -- а пора вставать
			l2h_print( script_name, " Rest end, HP ok ", hpp, "%, MP ok ", mpp, "%, standing\n" );
			sys_isResting = false; -- не отдыхаем!
			ensurestand();
			l2h_delay( 1500 );
		end
	end
end

function general_ai_follow()
	cfg_load_party();
	if follow_enable > 0 then
		follow( follow_name, follow_dist, 30 );
	end
end

function general_ai_party_auto_invite()
	local party = l2c_getParty(); --l2c_getParty() always returns not nil
	local iw = 0;
	local ip = 0;
	local w = "";
	for w in string.gmatch( party_auto_invite_names, "%w+" ) do
		if not isCharInParty( w, party ) then
			local info = l2c_getCharObjectIdByName( w );
			if info ~= nil then
				l2c_partySendInvite( w, party_loot );
				l2h_delay( 300 );
			end
		end
	end
	l2h_delay( 50 );
end

function general_ai_heal_self()
	local varname = "";
	local curHp, maxHp, curMp, maxMp, curCp, maxCp = l2c_getHPMPCPWeight();
	local usr = l2c_getStats();
	local cur_hpp = 100.0 * curHp / maxHp;
	local cur_mpp = 100.0 * curMp / maxMp;
	local cur_cpp = 100.0 * curCp / maxCp;
	local i = 0;
	if self_heal_use_item_nConds > 0 then
		for i=0, (self_heal_use_item_nConds - 1) do
			varname = string.format( "self_heal_use_item_[%d]_id", i );
			local itemID = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_item_[%d]_hp", i );
			local hpp = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_item_[%d]_mp", i );
			local mpp = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_item_[%d]_cp", i );
			local cpp = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_item_[%d]_hp_cond", i );
			local hp_cond = l2ccfg_getStr( varname );
			varname = string.format( "self_heal_use_item_[%d]_mp_cond", i );
			local mp_cond = l2ccfg_getStr( varname );
			varname = string.format( "self_heal_use_item_[%d]_cp_cond", i );
			local cp_cond = l2ccfg_getStr( varname );
			varname = string.format( "self_heal_use_item_[%d]_delayms", i );
			local delayms = l2ccfg_getInt( varname );
			local itemCnt = l2c_getItemCount( itemID );
			--l2h_print( "Use item ", itemID, " (we have count ", itemCnt, ") when HP ", hp_cond, " ", hpp, " and MP ", mp_cond, " ", mpp, " and CP ", cp_cond, " ", cpp, "\n" );
			if itemCnt > 0 then
				if isItemReadyForReuse( itemID, delayms ) then
					--l2h_print( "Cur hpp,mpp,cpp: ", cur_hpp, ",", cur_mpp, ",", cur_cpp, "\n" );
					local hp_cond_ok = false;
					local mp_cond_ok = false;
					local cp_cond_ok = false;
					if hp_cond == "eq" then hp_cond_ok = (cur_hpp == hpp) end
					if hp_cond == "lt" then hp_cond_ok = (cur_hpp < hpp) end
					if hp_cond == "le" then hp_cond_ok = (cur_hpp <= hpp) end
					if hp_cond == "gt" then hp_cond_ok = (cur_hpp > hpp) end
					if hp_cond == "ge" then hp_cond_ok = (cur_hpp >= hpp) end
					if mp_cond == "eq" then mp_cond_ok = (cur_mpp == mpp) end
					if mp_cond == "lt" then mp_cond_ok = (cur_mpp < mpp) end
					if mp_cond == "le" then mp_cond_ok = (cur_mpp <= mpp) end
					if mp_cond == "gt" then mp_cond_ok = (cur_mpp > mpp) end
					if mp_cond == "ge" then mp_cond_ok = (cur_mpp >= mpp) end
					if cp_cond == "eq" then cp_cond_ok = (cur_cpp == cpp) end
					if cp_cond == "lt" then cp_cond_ok = (cur_cpp < cpp) end
					if cp_cond == "le" then cp_cond_ok = (cur_cpp <= cpp) end
					if cp_cond == "gt" then cp_cond_ok = (cur_cpp > cpp) end
					if cp_cond == "ge" then cp_cond_ok = (cur_cpp >= cpp) end
					--l2h_print( "Hp_cond_ok: ", hp_cond_ok, "\n" );
					--l2h_print( "Mp_cond_ok: ", mp_cond_ok, "\n" );
					--l2h_print( "Cp_cond_ok: ", cp_cond_ok, "\n" );
					if hp_cond_ok and mp_cond_ok and cp_cond_ok then
						myUseItem( itemID );
					end
				end -- skill reuse
			end -- have this skill
		end
	end
	-- если настроены скилы которые можно заюзать на самоХил и при этом чар НЕ сидит отдыхает
	if (self_heal_use_skill_nConds > 0) and (not sys_isResting) then
		for i=0, (self_heal_use_item_nConds - 1) do
			varname = string.format( "self_heal_use_skill_[%d]_id", i );
			local skillID = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_skill_[%d]_hp", i );
			local hpp = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_skill_[%d]_mp", i );
			local mpp = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_skill_[%d]_cp", i );
			local cpp = l2ccfg_getInt( varname );
			varname = string.format( "self_heal_use_skill_[%d]_hp_cond", i );
			local hp_cond = l2ccfg_getStr( varname );
			varname = string.format( "self_heal_use_skill_[%d]_mp_cond", i );
			local mp_cond = l2ccfg_getStr( varname );
			varname = string.format( "self_heal_use_skill_[%d]_cp_cond", i );
			local cp_cond = l2ccfg_getStr( varname );
			local skillLvl = l2c_getSkillLevel( skillID );
			--l2h_print( "Use skill ", skillID, " (we have lvl ", skillLvl, ") when HP ", hp_cond, " ", hpp, " and MP ", mp_cond, " ", mpp, " and CP ", cp_cond, " ", cpp, "\n" );
			if skillLvl > 0 then
				if isSkillReadyForReuse( skillID ) then
					--l2h_print( "Cur hpp,mpp,cpp: ", cur_hpp, ",", cur_mpp, ",", cur_cpp, "\n" );
					local hp_cond_ok = false;
					local mp_cond_ok = false;
					local cp_cond_ok = false;
					if hp_cond == "eq" then hp_cond_ok = (cur_hpp == hpp) end
					if hp_cond == "lt" then hp_cond_ok = (cur_hpp < hpp) end
					if hp_cond == "le" then hp_cond_ok = (cur_hpp <= hpp) end
					if hp_cond == "gt" then hp_cond_ok = (cur_hpp > hpp) end
					if hp_cond == "ge" then hp_cond_ok = (cur_hpp >= hpp) end
					if mp_cond == "eq" then mp_cond_ok = (cur_mpp == mpp) end
					if mp_cond == "lt" then mp_cond_ok = (cur_mpp < mpp) end
					if mp_cond == "le" then mp_cond_ok = (cur_mpp <= mpp) end
					if mp_cond == "gt" then mp_cond_ok = (cur_mpp > mpp) end
					if mp_cond == "ge" then mp_cond_ok = (cur_mpp >= mpp) end
					if cp_cond == "eq" then cp_cond_ok = (cur_cpp == cpp) end
					if cp_cond == "lt" then cp_cond_ok = (cur_cpp < cpp) end
					if cp_cond == "le" then cp_cond_ok = (cur_cpp <= cpp) end
					if cp_cond == "gt" then cp_cond_ok = (cur_cpp > cpp) end
					if cp_cond == "ge" then cp_cond_ok = (cur_cpp >= cpp) end
					--l2h_print( "Hp_cond_ok: ", hp_cond_ok, "\n" );
					--l2h_print( "Mp_cond_ok: ", mp_cond_ok, "\n" );
					--l2h_print( "Cp_cond_ok: ", cp_cond_ok, "\n" );
					if hp_cond_ok and mp_cond_ok and cp_cond_ok then
						myTargetObjectTimeout( usr.objectID, 1 );
						myUseSkill( skillID );
					end
				end -- skill reuse
			end -- have this skill
		end -- for
	end
end

function general_ai_loop()
	general_ai_heal_self();
	general_ai_self_rest_sit_stand();
	general_ai_party_auto_invite();
	general_ai_follow();
end

while true do
	if l2c_is_INGAME() then
		if sys_should_exit() then break end
		-- call func in protected mode, any errors wil be inside this loop and never returned to C++ program
		--pcall( general_ai_loop );
		general_ai_loop();
		if sys_should_exit() then break end
	end
	l2h_delay( 300 );
	if sys_should_exit() then break end
end

l2h_print( "Sys AI script stopped\n" );
