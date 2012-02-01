script_name = "Archer_assist_pvp: ";

-- садиться и вставать при хп и мп
sit_below_hp = 0;
stand_above_hp = 0;
sit_below_mp = 0;
stand_above_mp = 0;

-- itemID каких соул- и спиритшотов юзать на автоюзе
soulshot_item = 1467; -- SSS
spiritshot_item = 0;

heal_itemID = 1539; -- Greater Healing potion
heal_item_use_hp = 95; -- 80%
heal_item_reuse_secs = 5; -- secs

mana_itemID = 728; -- Mana Potion
mana_item_use_mp = 85; -- 80%
mana_item_reuse_secs = 1; -- secs
-- тип зоны боя: квадрат или круг
combat_zone_type = "square"; -- квадрат
--combat_zone_type = "circle"; -- круг

-- тип начальной точки боя
-- "auto" где скрипт запущен, там и начало
-- "manual" Вручную: тогда вручную надо задать combat_start_x, combat_start_y, combat_start_z
combat_start_point = "manual";
combat_range = 3500;  -- Радиус боя, если круг, или половина длины стороны квадрата
-- центр зоны боя
combat_start_x = 135384;
combat_start_y = 23227;
combat_start_z = -3749;
combat_target_dz = 500; -- ограничение таргета по высоте +/- 500
combat_timeout_secs = 15; -- если в течении 15 сек хп моба не уменьшилось, отмена цели

assist_mode = true;
assist_member = "1110011001";
assist_only_mobs = false; -- if false, can assist in PvP to kill players
assist_dont_kill_party = true;

follow_name         = assist_member;
follow_distance     = 120;
follow_rand_time = 3;
kite_distance     = 900;
kite_range     = 400;

follow_snipe		= true;
snipe_id			= 313;

pickup_range = 400;
pickup_item_timeout = 750;

-- не атаковать мобов:
not_attack_mobs = { 21801, 21802, 21810 }; -- Treasure Chests

-- использовать ли теневое оружие, и какое
-- если включено, скрипт будет проверять, одето ли это оружие, и если нет,
-- будет пытаться одевать его. В инвентаре должен лежать запас оружия )
shadow_weapon_use = true;
shadow_weapon_iid = 10445; --  Icarus Bow Focus

skill_1 = 0; --101;--101; -- Stun Shot
skill_1_delay = 3000;

-- свои бафы
self_buffs = {
	131, -- Hawk Eye
	99, -- Rapid Shot
	--256, -- Accuracy
	416,
	415,
};

-------------------------------------------------------
-------------------------------------------------------
-------------------------------------------------------
-- system vars!!! do not edit !!!
-------------------------------------------------------
-------------------------------------------------------
sys_ignore_oid = 0;
sys_last_heal_item_use_time = 0;
sys_last_mana_item_use_time = 0;
-------------
sys_skill_1_used = false;
skill1_time = 0;
rand_time = 0;
x_rand = 0;
y_rand = 0;

-- функция закочит работу когда юзер закончит каст текущего скилла
function wait_end_of_cast()
	while l2c_isCastingNow() do l2h_delay( 50 ) end
end

-- пытается затаргетить objectID в течение timeoutSecs секунд
-- вернет true, если таргет удался
function myTargetObjectTimeout( objectID, timeoutSecs )
	if timeoutSecs == nil then timeoutSecs = 5 end
	local targetStartTime = l2h_time();
	local targetObjectId = 0;
	local timePassed = 0;
	targetObjectId = l2c_getTarget();
	if targetObjectId == objectID then return true end -- уже на таргете, ничо не надо делать
	l2c_action( objectID ); -- пытаемся затаргетить
	repeat -- проверяем в цикле, взят ли объект на цель
		targetObjectId = l2c_getTarget();
		l2h_delay( 100 );
		if sys_should_exit() then return end
		local curTime = l2h_time(); -- заодно смотрим по времени
		timePassed = curTime - targetStartTime;
	until (timePassed > timeoutSecs) or (targetObjectId == objectID) -- Либо таргет удался, либо истек таймаут
	if( targetObjectId ~= objectID ) then
		l2h_print( script_name, " ERROR: cannot target [", objectID, "]\n" );
		return false
	end
	return true
end

-- использует скилл и делает задержку 100мс ля уверенности что каст скилла начался
-- чиста для удобства. величина задержки зависит от лагов канала до сервера
function myUseSkill( skillID )
	l2c_useSkill( skillID, true )
	l2h_delay( 150 )
end

function isNotTank( objectId )
	local objectInfo = l2c_getObjectInfoByObjectId( objectId );
	if objectInfo == nil then return true end
	if( objectInfo.type == "pc" ) then
		if( objectInfo.classID == 4 ) then return false end
		if( objectInfo.classID == 5 ) then return false end
		if( objectInfo.classID == 6 ) then return false end
		if( objectInfo.classID == 19 ) then return false end
		if( objectInfo.classID == 20 ) then return false end
		if( objectInfo.classID == 21 ) then return false end
		if( objectInfo.classID == 32 ) then return false end
		if( objectInfo.classID == 33 ) then return false end
		if( objectInfo.classID == 34 ) then return false end
		if( objectInfo.classID == 90 ) then return false end
		if( objectInfo.classID == 91 ) then return false end
		if( objectInfo.classID == 99 ) then return false end
		if( objectInfo.classID == 100 ) then return false end
		if( objectInfo.classID == 106 ) then return false end
		if( objectInfo.classID == 107 ) then return false end
	end
	return true
end

function isDead( objectId )
	local objectInfo = l2c_getObjectInfoByObjectId( objectId );
	if objectInfo == nil then return true end
	if( objectInfo.type == "pc" ) then
		if( objectInfo.isAlikeDead == 1 ) then
			return true
		end
	end
	if( objectInfo.type == "npc" ) then
		if( objectInfo.isAlikeDead == 1 ) then
			return true
		end
	end
	if( objectInfo.type == "mob" ) then
		if( objectInfo.isAlikeDead == 1 ) then
			return true
		end
	end
	return false
end

function validate_target()
	local oid, curHp, maxHp = l2c_getTarget();
	if oid == 0 then return false end -- no target? :(
	local info = l2c_getObjectInfoByObjectId( oid );
	if not info then return false end -- O_o error
	if assist_only_mobs then
		if info.type == "mob" then return true end -- target must be attackable npc: mob
		--l2h_print( "Not mob!\n" );
		l2h_delay( 500 );
		return false; -- not mob on target
	end
	-- not assist ONLY mobs
	if assist_dont_kill_party then
		local party = l2c_getParty();
		local nMembers = #party;
		local i = 0;
		local target_is_party_member = false;
		if nMembers > 0 then
			for i=1, nMembers do
				if party[i].objectID == oid then target_is_party_member = true end
			end
		end
		if target_is_party_member then return false end -- DONT allow to kill party members
	end
	return true; -- no matter what is on target
end

function isAllowedAttackMob( npcID )
	local n = #not_attack_mobs
	if n < 1 then return true end
	local i
	for i=1,n do
		if npcID == not_attack_mobs[i] then return false end
	end
	return true
end

function use_heal_item()
	local curHp, maxHp, curMp, maxMp, curCp, maxCp = l2c_getHPMPCPWeight()
	local hp_percent = 100.0 * curHp / maxHp
	local mp_percent = 100.0 * curMp / maxMp
	local curTime = l2h_time()
	-- use heal item
	if (heal_itemID>0) then
		if hp_percent <= heal_item_use_hp then
			if (curTime - sys_last_heal_item_use_time) > heal_item_reuse_secs then
				l2h_print( "Self heal: use item!\n" )
				l2c_useItem( heal_itemID );
				sys_last_heal_item_use_time = curTime;
			end
		end
	end
	if (mana_itemID>0) then
		if mp_percent <= mana_item_use_mp then
			if (curTime - sys_last_mana_item_use_time) > mana_item_reuse_secs then
				l2h_print( "Self heal: use item!\n" )
				l2c_useItem( mana_itemID );
				sys_last_mana_item_use_time = curTime;
			end
		end
	end
end

function check_shadow_weapon()
	if not shadow_weapon_use then return end
	local rhand_iid, rhand_oid = l2c_getPaperdollItem( 9 ) -- 9 is PAPERDOLL_RHAND
	if rhand_iid == shadow_weapon_iid then return end
	l2h_print( "Incorrect weapon equipped! Equip shadow!\n" )
	l2h_print( "Current: Rhand: itemID [", rhand_iid, "]  oid [", rhand_oid, "]\n" )
	l2c_useItemByObjectId( rhand_oid ) -- disarm current weapon
	l2h_delay( 250 )
	local cnt = l2c_getItemCount( shadow_weapon_iid )
	if cnt < 1 then
		l2h_print( "ERROR: No shadow weapon left!\n" )
		l2h_soundAlert()
		return
	end
	l2c_useItem( shadow_weapon_iid ) -- equip weapon
	l2h_delay( 250 )
	rhand_iid, rhand_oid = l2c_getPaperdollItem( 9 ) -- 9 is PAPERDOLL_RHAND
	-- wait until weapon is equipped
	while rhand_iid ~= shadow_weapon_iid do
		l2h_delay( 100 )
		rhand_iid, rhand_oid = l2c_getPaperdollItem( 9 ) -- 9 is PAPERDOLL_RHAND
	end
	l2h_print( "Equipped!\n" )
end

function kill_target()
	local oid, tcurHp, tmaxHp = l2c_getTarget()
	local objectInfo = l2c_getObjectInfoByObjectId(oid)
	local selfInfo = l2c_getStats()
	local curHp, maxHp, curMp, maxMp, curCp, maxCp = l2c_getHPMPCPWeight()
	local hp_percent = 100.0 * curHp / maxHp
	local mp_percent = 100.0 * curMp / maxMp
	local target_target = 0
	if objectInfo ~= nil then 
		if( objectInfo.type == "pc" ) then
			target_target = objectInfo.targetObjectID
		end
	end
	local startHp = curHp -- remember mob start HP
	--l2c_action( oid ) -- action to current target
	check_shadow_weapon()
--l2h_print(target_target)
 -- for sure, request attack current target
 --	if (not (isDead( oid ))) and selfInfo.objectID == target_target then
	use_heal_item()
	if (not (isDead( oid ))) then
		if (skill_1 > 0) and (l2c_getSkillReuseLeft(skill_1) == 0) and ((l2h_timeMsec()-skill1_time) > skill_1_delay) and (mp_percent > 20) then
			skill1_time = l2h_timeMsec()
			myUseSkill( skill_1 )
			if not l2c_isCastingNow() then
				skill1_time = 0
			else
				wait_end_of_cast()
			end
		end
	end
	l2c_attack()
	l2h_delay( 400 )
	if (kite_distance > 0) and have_target() then
		x,y,z = l2c_getPos() -- получим наши координаты
		c = l2c_getObjectInfoByObjectId( oid ) -- координаты цели
		if c == nil then return end
		m = l2c_getObjectInfoByObjectId( l2c_getCharObjectIdByName( follow_name ) ) -- координаты МА
		--if m == nil then return end
		local mx = x
		local my = y
		local mx1 = x
		local my1 = y
		rand = 0.2
		if (calc_dist(c.x-x,c.y-y)+150<kite_distance) or (calc_dist(m.x-x,m.y-y)>kite_range) then
			if (x-c.x) ~= 0 then 
				local angle = math.atan((y-c.y)/(x-c.x))
				if x<c.x then angle = angle + math.pi end
				mx = x+150 * math.cos(angle + math.random( (0-rand), rand ))
				my = y+150 * math.sin(angle + math.random( (0-rand), rand ))
			else
				if y-c.y > 0 then my = y + 150 else my = y - 150 end
			end
			mx1 = mx
			my1 = my
			if m ~= nil then
				if calc_dist(m.x-mx,m.y-my)>kite_range then
                    --l2h_print( mx," ",my," ",m.x," ",m.y, "\n" )
                    local cx = m.x + kite_range * (mx-m.x) / calc_dist(m.x-mx,m.y-my)
                    local cy = m.y + kite_range * (my-m.y) / calc_dist(m.x-mx,m.y-my)
                    mx = x + 150 * (cx-x) / calc_dist(cx-x,cy-y)
                    my = y + 150 * (cy-y) / calc_dist(cx-x,cy-y)
                    --l2h_print( mx," ",my," ",x," ",y," ",cx," ",cy," ", "\n" )
				else
					--l2h_print( "close ", calc_dist(m.x-mx,m.y-my), "\n" )
				end
			else
				--l2h_print( "No MA!\n" )
			end
			if (calc_dist(c.x-mx,c.y-my)<kite_distance) and (calc_dist(mx1-mx, my1-my) < 240) then 
			l2c_moveTo( mx, my, z ) 
			else
			--l2h_print( "far ", calc_dist(c.x-mx,c.y-my), "\n" )
			end
			wait_end_of_cast()
		end
	end
	local startSec = l2h_time() -- remember start time
	local curSec = 0

	--l2h_print( " Killed target!\n" )
	-- kill was OK, cancel ignored mob
	sys_ignore_oid = 0;
	if sys_should_exit() then return end
	sys_skill1_1_used = false;
end


function rest()
	local curHp, maxHp, curMp, maxMp, curCp, maxCp = l2c_getHPMPCPWeight()
	local prevHp = curHp
	local hp_percent = 100.0 * curHp / maxHp
	local mp_percent = 100.0 * curMp / maxMp
	local isSitting, isRunnig = l2c_getSitRun()
	repeat
		if sys_should_exit() then return end
		curHp, maxHp, curMp, maxMp, curCp, maxCp = l2c_getHPMPCPWeight()
		if curHp < prevHp then
			l2h_print( "Rest interrupted (HP)?\n" )
			l2h_soundAlert()
			return
		end
		prevHp = curHp
		hp_percent = 100.0 * curHp / maxHp
		mp_percent = 100.0 * curMp / maxMp
		l2h_delay( 1000 )
		isSitting, isRunnig = l2c_getSitRun()
		if not isSitting then
			l2h_print( "Rest interrupted (stand)?\n" )
			l2h_soundAlert()
			return
		end
		l2h_delay( 200 )
		local hp_good = false
		local mp_good = false
		if hp_percent >= stand_above_hp then hp_good = true end
		if mp_percent >= stand_above_mp then mp_good = true end
	until hp_good and mp_good
	l2h_print( "Rest end, HP ok ", hp_percent, "%, MP ok ", mp_percent, "%, standing\n" )
	ensurestand()
	l2h_delay( 1500 )
end

function check_my_hp_mp()
	local curHp, maxHp, curMp, maxMp, curCp, maxCp = l2c_getHPMPCPWeight()
	local hp_percent = 100.0 * curHp / maxHp
	local mp_percent = 100.0 * curMp / maxMp
	local hp_low = false
	local mp_low = false
	if hp_percent < sit_below_hp then hp_low = true end
	if mp_percent < sit_below_mp then mp_low = true end
	if hp_low or mp_low then
		l2h_print( "We must sit! HP ", hp_percent, "%, MP ", mp_percent, "%\n" )
		l2c_sitStand()
		l2h_delay( 500 )
		rest()
	--[[else
		l2h_print( "No need to rest...\n" )]]--
	end
end

function ensurestand()
	local isSitting, isRunnig = l2c_getSitRun()
	if isSitting then
		l2c_sitStand()
		l2h_delay( 1000 )
	end
end

function ensuresit()
	local isSitting, isRunnig = l2c_getSitRun()
	if not isSitting then
		l2c_sitStand()
		l2h_delay( 1000 )
	end
end

function ensurerun()
	local isSitting, isRunnig = l2c_getSitRun()
	if not isRunnig then
		l2c_runWalk()
		l2h_delay( 50 )
	end
end

function ensurewalk()
	local isSitting, isRunnig = l2c_getSitRun()
	if isRunnig then
		l2c_runWalk()
		l2h_delay( 50 )
	end
end

function calc_dist( dx, dy )
	return math.sqrt( dx*dx + dy*dy )
end

function isPointInFightZone( px, py, pz )
	-- first check z-delta
	local dz = math.abs( pz - combat_start_z )
	if dz > combat_target_dz then return false end
	local dx = math.abs( px - combat_start_x )
	local dy = math.abs( py - combat_start_y )
	-- for square zone
	if combat_zone_type == "square" then
		if (dx < combat_range) and (dy < combat_range) then return true end
		--l2h_print( "isPointInFightZone(): no (square)\n" )
		return false
	end
	-- for circle zone
	if combat_zone_type == "circle" then
		local r = calc_dist( dx, dy )
		if r < combat_range then return true end
		--l2h_print( "isPointInFightZone(): no (circle)\n" )
		return false
	end
	l2h_print( "ERROR: unknown combat zone type!\n" )
	return false
end

function target_nearest_attackable()
	local x, y, z = l2c_getPos()
	local mobs = l2c_getVisibleMobs()
	local nmobs = #mobs
	local i = 0
	local dist = 0
	local min_dist = 20000
	local min_objectID = 0
	local min_name = ""
	if nmobs == 0 then
		l2h_print( "No mobs?\n" );
		l2h_delay( 200 );
		return
	end
	for i=1, nmobs do
		local mobinfo = l2c_getObjectInfoByObjectId( mobs[i] )
		-- only valid mobs and which are not ignored
		if (mobinfo ~= nil) and (mobs[i] ~= sys_ignore_oid) then
			-- check if mob is in fight range
			local isInRange = isPointInFightZone( mobinfo.x, mobinfo.y, mobinfo.z )
			-- check if we are allowed to attack this mob
			local isAllowed = isAllowedAttackMob( mobinfo.templateID )
			local isAlive = false
			if mobinfo.isAlikeDead == 0 then isAlive = true end
			-- only count alive mobs, that are in fight range and allowed to attack
			if ( isAlive and isInRange and isAllowed ) then
				local dx = mobinfo.x - x
				local dy = mobinfo.y - y
				dist = calc_dist( dx, dy )
				if dist < min_dist then
					min_dist = dist
					min_objectID = mobinfo.objectID
					min_name = mobinfo.name
				end
			end
		end
	end
	if min_objectID == 0 then
		l2h_print( "Error: cannot target: min_objectID = 0!, no mons in range?\n" )
		l2h_delay( 1000 )
		-- begin move to combat start point
		l2c_moveTo( combat_start_x, combat_start_y, combat_start_z )
		return
	end
	l2h_print( "Target: [", min_name, "] " )
	l2c_action( min_objectID )
	l2h_delay( 250 )
end

function targetByAssist()
	if not assist_mode then return end
	local party = l2c_getParty();
	if #party == 0 then
		l2h_print( "Cannot assist, no party!\n" );
		l2h_delay( 200 );
		return
	end
	local i = 0;
	local targetObjectID = 0;
	for i=1, #party do
		if party[i].name == assist_member then
			targetObjectID = party[i].targetObjectID;
		end
	end
	local oid, curHp, maxHp = l2c_getTarget();
	if not ( targetObjectID == 0 ) then
		if targetObjectID == l2c_getCharObjectIdByName( follow_name ) then 
			--l2c_targetCancel()
			follow( follow_name, 120 ) 
		end
		if oid == 0 then
			myTargetObjectTimeout( targetObjectID, 1 );
		else if isDead( oid ) then
			--l2h_print( "DEAD\n" )
			myTargetObjectTimeout( targetObjectID, 1 );	
			else if isNotTank( targetObjectID ) then
				myTargetObjectTimeout( targetObjectID, 1 );		
				end
			--l2h_print( "LIVE\n" )
			end
		end
	end
end

function have_target()
	local oid, curHp, maxHp = l2c_getTarget();
	if oid == 0 then return false end -- no target? :(
	local info = l2c_getObjectInfoByObjectId( oid );
	if not info then return false end -- O_o error
	local selfInfo = l2c_getStats()
	if assist_only_mobs then
		if info.type == "mob" then return true end -- target must be attackable npc: mob
		return false; -- not mob on target
	end
	if selfInfo.objectID == oid then return false end
	if isDead(oid) then return false end
	-- not assist ONLY mobs
	if assist_dont_kill_party then
		local party = l2c_getParty();
		local nMembers = #party;
		local i = 0;
		local target_is_party_member = false;
		if nMembers > 0 then
			for i=1, nMembers do
				if party[i].objectID == oid then target_is_party_member = true end
			end
		end
		if target_is_party_member then return false end -- DONT allow to kill party members
	end
	return true; -- no matter what is on target
end

function target_mob()
	if assist_mode then
		targetByAssist()
	else
		target_nearest_attackable()
	end
end

function pickup_nearest_items( pickup_dist )
	local x, y, z = l2c_getPos()
	local items = l2c_getVisibleItems()
	local nitems = #items
	local dist = 0
	local i = 0
	if nitems == 0 then return end
	l2c_targetCancel()
	l2h_delay( 50 )
	l2h_print( "Pick: " )
	for i=1, nitems do
		if sys_should_exit() then return end
		local iteminfo = l2c_getObjectInfoByObjectId( items[i] )
		if iteminfo ~= nil then
			local dx = iteminfo.x - x;
			local dy = iteminfo.y - y;
			dist = calc_dist( dx, dy )
			if dist < pickup_dist then
				--l2h_print( "[", iteminfo.name, "] ", iteminfo.count, ", " )
				--l2c_moveTo( iteminfo.x, iteminfo.y, iteminfo.z )
				l2c_action( items[i] )
				--l2c_action( items[i] )
				l2h_delay( pickup_item_timeout )
			end
		end
	end
	l2h_print( " Pick end\n" )
end

function follow( name, distance )
	-- получим objectID чара
	randomness = 60
	local objectID = l2c_getCharObjectIdByName( name )
	-- а есть ли такой чар вообще?
	if objectID == nil then
		l2h_print( "Character [", name, "] is not visible!\n" )
		return
	end
	if randomness >= distance then randomness = distance/2 end
	local x, y, z -- наши координаты будут тут
	local gox, goy, goz
	local c       -- инфо о чаре будет тут
	-- а надо ли нам следовать?
	if (l2h_time()-rand_time) > follow_rand_time then
		rand_time = l2h_time()
		x_rand = math.random( (0-randomness), randomness )
		y_rand = math.random( (0-randomness), randomness )
	end
	x,y,z = l2c_getPos() -- получим наши координаты
	c = l2c_getObjectInfoByObjectId( objectID ) -- инфо о цели "преследования"
	if (c.xDst ~= 0x7FFFFFFF) and (c.yDst ~= 0x7FFFFFFF) then
		gox=c.xDst
		goy=c.yDst
		goz=c.zDst
	else
		gox=c.x
		goy=c.y
		goz=c.z
	end
	if (math.abs(gox - x) > distance) or (math.abs(goy - y) > distance) then
		--if (c.xDst ~= 0x7FFFFFFF) and (c.yDst ~= 0x7FFFFFFF) and (math.abs(c.xDst - x) > distance) and (math.abs(c.yDst - y) > distance) then
		l2c_moveTo( gox + x_rand, goy + y_rand, goz ) -- двинемся к нему		
	else
		return -- да мы и так недалеко от цели
	end
	-- цикл следования
	repeat
		if sys_should_exit() then return end -- юзер нажал "Стоп скрипт"
		-- получим инфо о чаре
		c = l2c_getObjectInfoByObjectId( objectID )
		--l2h_print( "Following [", name, "] to (", c.x, ",", c.y, ",", c.z, ")\n" )
		if (l2h_time()-rand_time) > follow_rand_time then
			rand_time = l2h_time()
			x_rand = math.random( (0-randomness), randomness )
			y_rand = math.random( (0-randomness), randomness )
		end
		if (c.xDst ~= 0x7FFFFFFF) and (c.yDst ~= 0x7FFFFFFF) then
			gox=c.xDst
			goy=c.yDst
			goz=c.zDst
		else
			gox=c.x
			goy=c.y
			goz=c.z
		end
		l2c_moveTo( gox + x_rand, goy + y_rand, goz ) -- двинемся к нему
		l2h_delay( 300 ) -- подождем
		x,y,z = l2c_getPos() -- получим наши координаты
		-- проверим разницу наших координат и координат чара.
		-- если разница больше дистанции следования, цикл следования повторится
	until ((math.abs(gox - x) < distance) or (math.abs(goy - y) < distance) or (math.abs(goz - z) < distance))
end

-- вернет true, если баф skillID присутсвует в массиве have_buffs
function buffIsPresent( skillID, have_buffs )
	if not have_buffs then return false end
	local i
	for i=1, #have_buffs do
		if have_buffs[i].skillID == skillID then return true end
	end
	return false
end

-- проверить свои бафы и ребафнуть недостающие
function check_self_buffs()
	local have_buffs = l2c_getBuffs()
	local ma_buffs = l2c_getPartyMemberBuffs(l2c_getCharObjectIdByName(follow_name))
	local j
	local have_snipe = false
	for j=1, #ma_buffs do
		if ma_buffs[j].skillID == snipe_id then
			have_snipe = true
		end
		--if 
	end
	local user = l2c_getStats() -- нам нужен свой objectID чтобы затаргетить
	local i
	if follow_snipe then
		if (not have_snipe) and ( buffIsPresent( snipe_id, have_buffs )) then
			l2c_buffCancel( snipe_id )
			--l2h_print("Cancel!\n")
		end
		if have_snipe and (not buffIsPresent( snipe_id, have_buffs )) then
			--myTargetObjectTimeout( user.objectID, 5 )
			--l2h_print("Snipe!\n")
			myUseSkill( snipe_id )
			wait_end_of_cast()
		end
	end
	for i=1, #self_buffs do
		if sys_should_exit() then return end -- юзер нажал "Стоп скрипт"
		if not buffIsPresent( self_buffs[i], have_buffs ) then
			l2h_print( script_name, " Need self rebuff [", self_buffs[i], "]\n" )
			myTargetObjectTimeout( user.objectID, 5 )
			myUseSkill( self_buffs[i] )
			wait_end_of_cast()
		end
		--if 
	end
end

function fight()
	target_mob()
	if validate_target() then
		if sys_should_exit() then return end
		kill_target()
		if sys_should_exit() then return end
		--pickup_nearest_items( 400 )
		--pickup_nearest_items( pickup_range )
		--if sys_should_exit() then return end
		--check_my_hp_mp()
		--if sys_should_exit() then return end
	end
end

function setup_soulshots()
	if soulshot_item > 0 then l2c_autoSoulshot( soulshot_item, true ) end
	if spiritshot_item > 0 then l2c_autoSoulshot( spiritshot_item, true ) end
end

function setup_fight_zone()
	if assist_mode then
		l2h_print( "Combat mode: assist\n" );
		return
	end
	l2h_print( "Combat mode: leader\n" )
	if combat_start_point == "auto" then
		combat_start_x, combat_start_y, combat_start_z = l2c_getPos()
	end
	l2h_print( "Combat start point type: ", combat_start_point, "\n" );
	l2h_print( "Combat start at: (", combat_start_x, ", ", combat_start_y, ", ", combat_start_z, "\n" );
	l2h_print( "Combat zone type: ", combat_zone_type, "\n" );
end

--  script start
ensurestand()
ensurerun()
check_shadow_weapon() -- equip weapon before using soulshots/spiritshots
setup_soulshots()
setup_fight_zone()

while true do
	check_self_buffs()
	if assist_mode then 
		if (kite_distance == 0) or (not (have_target())) then
			follow( follow_name, follow_distance )
		end
	end
	fight()
	if sys_should_exit() then break end
	if not l2c_is_INGAME() then break end
end

l2h_print( "Script assist stopped\n" )
