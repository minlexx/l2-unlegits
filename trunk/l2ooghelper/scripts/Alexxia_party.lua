-----------------------------
--    ��������� �������
-----------------------------
script_name = "Alexxia_BP_Pty: ";
-- ��������� ����-���������
--soulshot_itemID     = 1835; -- Soulshot: No Grade
--soulshot_itemID     = 1463; -- Soulshot: D Grade
--soulshot_itemID     = 1464; -- Soulshot: C Grade
--soulshot_itemID     = 1465; -- Soulshot: B Grade
soulshot_itemID     = 0; -- Soulshot: A Grade
--soulshot_itemID     = 1467; -- Soulshot: S Grade
--spiritshot_itemID   = 3947; -- Blessed Spiritshot: No Grade
--spiritshot_itemID   = 3948; -- Blessed Spiritshot: D Grade
--spiritshot_itemID   = 3949; -- Blessed Spiritshot: C Grade
--spiritshot_itemID   = 3950; -- Blessed Spiritshot: B Grade
--spiritshot_itemID   = 3951; -- Blessed Spiritshot: A Grade
spiritshot_itemID   = 3952; -- Blessed Spiritshot: S Grade
--  ��������� ����������
follow_enable       = true;
follow_name         = "Semirra";
follow_distance     = 120;
follow_rand_time = 3;
--  ��������� ����
heal_self_enable    = true;
heal_party_enable   = true;
heal_party_distance = 600;
heal_percent        = 85;
--heal_skill          = 1011; -- Heal
heal_skill          = 1218; -- Major Heal
heal_battle_percent = 1;
--heal_battle_skill   = 1015; -- Battle Heal
heal_battle_skill   = 1016; -- Greater Battle heal
-- ����� Healing Potion

heal_itemID = 1539; -- Greater Healing potion
heal_item_use_hp = 95; -- 80%
heal_item_reuse_secs = 5; -- secs

mana_itemID = 728; -- Mana Potion
mana_item_use_mp = 85; -- 80%
mana_item_reuse_secs = 1; -- secs
-- ��������� �������
recharge_enable     = false;
recharge_mp_percent = 70;
recharge_skill      = 1013; -- Recharge

-- ���� ����
self_buffs_auto = true;
self_buffs = {
	--3138, -- Item Skill: Heal Empower
	--1078, -- Concentration
	--1040, -- Shield
	--1204, -- Wind Walk
	--1035, -- Mental Shield
	--1044, -- Regeneration
	--1397, -- Clarity
	--1062, -- Berserker spirit
	--1311, -- Body of Avatar
	--1259  -- Resist Shog
	1353,
	1323
};


party_buff_enable = true;
party_buff_autodetect = true;
party_buff_distance = 400;
party_rebuff_secs = 1170; -- ����� ����� ��� �������� ����; �� ������������ ���� ���������� ����������
party_buffs = {
	--1068, -- Might
	--1040, -- Shield
	--1087, -- Agility
	--1043, -- Holy Weapon
	--1035, -- Mental Shield
	--1204, -- Wind Walk
	--1044, -- Regeneration
	--1397, -- Clarity
	--1062, -- Berserker spirit
	--1259  -- Resist Shog
	1353,
	1323
};


--- CARDINAL UNIQUE ---
-- ������ ����� ������������� �����:
-- 1) � ���� ������� [balance_life_min_good_players] ����� ������� ����� ��� � [balance_life_good_hp] % HP
-- 2) � ���� �������� [balance_life_max_bad_players] ��������� ������� � [balance_life_bad_hp] % HP
-- 3) ���� �� ����� [balance_life_min_mp_limit] %
balance_life_enable = false;
balance_life_good_hp = 90;
balance_life_bad_hp = 40;
balance_life_min_good_players = 3;
balance_life_max_bad_players = 1;
balance_life_min_mp_limit = 25; -- self mp %
balance_life_skill = 1335; -- Balance Life

group_heal_enable = true;
group_heal_bad_hp = 90;
group_heal_min_bad_players = 3;
group_heal_skill = 1402; -- major group heal
--- CARDINAL UNIQUE ---


-------------------------------------------------------
-------------------------------------------------------
-------------------------------------------------------
-- system vars!!! do not edit !!!
-------------------------------------------------------
-------------------------------------------------------
sys_should_exit = false;
sys_last_party_buff_time = 0;
sys_last_heal_item_use_time = 0;
sys_last_mana_item_use_time = 0;
rand_time = 0;
x_rand = 0;
y_rand = 0;

-- ������� ������� ������ ����� ���� �������� ���� �������� ������
function wait_end_of_cast()
	while l2c_isCastingNow() do l2h_delay( 50 ) end
end

-- �������� ����������� objectID � ������� timeoutSecs ������
-- ������ true, ���� ������ ������
function myTargetObjectTimeout( objectID, timeoutSecs )
	local targetStartTime = l2h_time();
	l2c_action( objectID );
	local targetObjectId = 0;
	local timePassed = 0;
	repeat
		targetObjectId = l2c_getTarget();
		l2h_delay( 100 );
		if sys_should_exit then return end
		local curTime = l2h_time();
		timePassed = curTime - targetStartTime;
	until (timePassed > timeoutSecs) or (targetObjectId == objectID)
	if( targetObjectId ~= objectID ) then
		l2h_print( script_name, " ERROR: cannot target [", objectID, "]\n" );
		return false
	end
	return true
end

-- ���������� ����� � ������ �������� 100�� �� ����������� ��� ���� ������ �������
-- ����� ��� ��������. �������� �������� ������� �� ����� ������ �� �������
function myUseSkill( skillID )
	l2c_useSkill( skillID )
	l2h_delay( 150 )
end

-- �������
-- ��������� �� ����� [name] �� ��������� [distance]
-- ���� �������� ��� ��� ��������� � �������� [distance], follow �� �������������
-- randomness - �����, � �������� �������� ���������� ����������� �� ����. ������������� 1-100
function follow( name, distance )
	-- ������� objectID ����
	randomness = 60
	local objectID = l2c_getCharObjectIdByName( name )
	-- � ���� �� ����� ��� ������?
	if objectID == nil then
		l2h_print( "Character [", name, "] is not visible!\n" )
		return
	end
	if randomness >= distance then randomness = distance/2 end
	local x, y, z -- ���� ���������� ����� ���
	local gox, goy, goz
	local c       -- ���� � ���� ����� ���
	-- � ���� �� ��� ���������?
	if (l2h_time()-rand_time) > follow_rand_time then
		rand_time = l2h_time()
		x_rand = math.random( (0-randomness), randomness )
		y_rand = math.random( (0-randomness), randomness )
	end
	x,y,z = l2c_getPos() -- ������� ���� ����������
	c = l2c_getObjectInfoByObjectId( objectID ) -- ���� � ���� "�������������"
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
		l2c_moveTo( gox + x_rand, goy + y_rand, goz ) -- �������� � ����		
	else
		return -- �� �� � ��� �������� �� ����
	end
	-- ���� ����������
	repeat
		if sys_should_exit then return end -- ���� ����� "���� ������"
		-- ������� ���� � ����
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
		l2c_moveTo( gox + x_rand, goy + y_rand, goz ) -- �������� � ����
		l2h_delay( 300 ) -- ��������
		x,y,z = l2c_getPos() -- ������� ���� ����������
		-- �������� ������� ����� ��������� � ��������� ����.
		-- ���� ������� ������ ��������� ����������, ���� ���������� ����������
	until ((math.abs(gox - x) < distance) or (math.abs(goy - y) < distance) or (math.abs(goz - z) < distance))
end

-- �������
-- �������� ������ ����� ���� �� ������������� ���� ��� �������
-- ���������� �� ������� check_party()
function check_member_HP_and_heal( member )
	wait_end_of_cast() -- ���������, ���� ���������� ���� �������� ������
	local hp_percent = 100.0 * member.curHp / member.maxHp; -- ��������� % HP �������
	local mp_percent = 100.0 * member.curMp / member.maxMp; -- ��������� % MP �������
	if heal_party_enable then -- ������� ��� ����
		if hp_percent < heal_battle_percent then -- ���� ����������
			l2h_print( script_name, ": ", member.name.." needs BATTLE healing! ", hp_percent, "% HP!\n" );
			follow( member.name, heal_party_distance, 50 ); -- ����� ������ ������?
			myTargetObjectTimeout( member.objectID, 5 );
			myUseSkill( heal_battle_skill );
			wait_end_of_cast()
		end
		if hp_percent < heal_percent then -- ���� ������
			l2h_print( script_name, ": ", member.name.." needs healing! ", hp_percent, "% HP!\n" );
			follow( member.name, heal_party_distance, 50 ); -- ����� ������ ������?
			myTargetObjectTimeout( member.objectID, 5 );
			myUseSkill( heal_skill );
			wait_end_of_cast()
		end
	end
	if recharge_enable and (mp_percent < recharge_mp_percent) then
		l2h_print( script_name, ": ", member.name.." needs recharge! ", mp_percent, "% MP!\n" );
		follow( member.name, heal_party_distance, 50 ); -- ����� ������ ������?
		myTargetObjectTimeout( member.objectID, 5 );
		myUseSkill( recharge_skill );
		wait_end_of_cast()
	end
end

-- �������
-- ��������� ���� �� ������������� ����
function check_party()
	local party = l2c_getParty(); -- ������� ����
	local nMembers = #party       -- ������� � ��� � ����?
	local i
	if nMembers == 0 then -- ������ ���? �_�
		l2h_print( script_name, " No members in party!\n" )
		l2h_delay( 1000 )
		return -- ������ ���!
	end
	for i=1, nMembers, 1 do
		if sys_should_exit then return end -- ���� ����� "���� ������"
		check_member_HP_and_heal( party[i] ) -- �������� ������ i-�� �������
	end
end



-- ������� ������ true, ���� ���� � ����
function is_in_party()
	local party = l2c_getParty()
	local nMembers = #party
	if nMembers < 1 then return false end -- no party
	return true
end

-- ������ true, ���� ��� skillID ����������� � ������� have_buffs
function buffIsPresent( skillID, have_buffs )
	if not have_buffs then return false end
	local i
	for i=1, #have_buffs do
		if have_buffs[i].skillID == skillID then return true end
	end
	return false
end


--[[ table l2c_getPartyMemberBuffs( int objectID )
��������� ������ (�������) � ����� lua (������������ � �������), ���������� ���������� � ����� ������ ����� ����.
������ ������� ������� - ��������� (�������) � ������, ������ ��, ��� � ������������ �������� l2c_getBuffs().
���� ������ ���, ������ ����� ����.
������ ������� - ��������� � ������: skillName, skillID, skillLvl, durationSecs
��������� ��� ������, ID ������, ��� �������, ���������� ����� � ��������. ]]--
function rebuff_party_autodetect()
	if not party_buff_enable then return end
	if not party_buff_autodetect then return end
	local party = l2c_getParty(); -- ������� ���� ����
	local nMembers = #party; -- ����� � ��� ������ � ����?
	if nMembers < 1 then return end -- ������ ��� � ���� :(
	-- ��� ������� ������� ������� ��� ������ �����. ������ �����������
	--l2h_print( script_name, " Rebuff party autodetect!\n" )
	local i = 0;
	for i=1, nMembers do
		if sys_should_exit then return end -- ���� ����� "���� ������"
		local member_buffs = l2c_getPartyMemberBuffs( party[i].objectID );
		local hp_percent = 100.0 * party[i].curHp / party[i].maxHp; -- ��������� % HP �������
		-- ��������� �� ���� ���� �����
		local ii = 0;
		for ii=1, #party_buffs do
			if sys_should_exit then return end -- ���� ����� "���� ������"
			-- ������� �� ii-� ��� �� �������?
			if not buffIsPresent( party_buffs[ii], member_buffs ) and hp_percent>1 then
				l2h_print( script_name, " Rebuff party auto: member [", party[i].name,
					"] is missing buff [", party_buffs[ii], "]\n" );
				myTargetObjectTimeout( party[i].objectID, 3 ); -- ����� ������� �� ����
				follow( party[i].name, party_buff_distance, 50 ); -- ��������� � ������� �� ���������� ����
				l2h_print( script_name, " Rebuff party auto: use [", party_buffs[ii], "]\n" );
				myUseSkill( party_buffs[ii] ); -- ��������� ��������� ���
				wait_end_of_cast();             -- ��������� ����� ����� �����
			end
		end
	end
	--l2h_print( script_name, " Rebuff party autodetect end.\n" )
end


-- �������� ����� ���� �� �������. ������� ���� ����� ���� �����
-- ���������� �� ������� rebuff_party_by_timer()
function rebuff_party_member_by_timer( member )
	myTargetObjectTimeout( member.objectID, 5 ); -- ����� ������� �� ����
	-- ������� ���
	local i
	for i=1, #party_buffs do
		if sys_should_exit then return end -- ���� ����� "���� ������"
		follow( member.name, party_buff_distance, 50 ); -- ��������� � ������� �� ���������� ����
		l2h_print( script_name, " Rebuff party timer: use [", party_buffs[i], "]\n" );
		myUseSkill( party_buffs[i] ); -- ��������� ��������� ���
		wait_end_of_cast();             -- ��������� ����� ����� �����
	end
end

-- �������� ��� ����, ���� ������� �����
-- ���������� �� ������� rebuff_party()
function rebuff_party_by_timer()
	if not party_buff_enable then return end
	local party = l2c_getParty();
	local nMembers = #party;
	if nMembers < 1 then return end -- no party
	local curTime = l2h_time(); -- ������� ����� � ���.
	local timePassed = curTime - sys_last_party_buff_time; -- ����� ��� ������ � ���������� �������
	if timePassed < party_rebuff_secs then return end -- ��� �� ����
	-- ���� �������
	l2h_print( script_name, " Rebuff party by timer!\n" )
	sys_last_party_buff_time = curTime; -- �������� ����� ����
	local i
	for i=1, nMembers do
		if sys_should_exit then return end -- ���� ����� "���� ������"
		rebuff_party_member_by_timer( party[i] )
	end
	l2h_print( script_name, " Rebuff party by timer end.\n" )
end

-- �������� ����, ������� �������������� �������, ������� �� �������� - ���������� ��� �� �������
function rebuff_party()
	if #party_buffs < 1 then
		l2h_print( script_name, " party buffs not configured!\n" )
		return
	end
	if party_buff_autodetect then
		rebuff_party_autodetect()
	else
		rebuff_party_by_timer()
	end
end


-- ��������� ���� ���� � ��������� �����������
function check_self_buffs()
	local have_buffs = l2c_getBuffs()
	local user = l2c_getStats() -- ��� ����� ���� objectID ����� �����������
	local i
	for i=1, #self_buffs do
		if sys_should_exit then return end -- ���� ����� "���� ������"
		if not buffIsPresent( self_buffs[i], have_buffs ) then
			l2h_print( script_name, " Need self rebuff [", self_buffs[i], "]\n" )
			myTargetObjectTimeout( user.objectID, 5 )
			myUseSkill( self_buffs[i] )
			wait_end_of_cast()
		end
	end
end

-- ��������� �� ���� �� ��� ����������
-- ���������� ���� ��� ������� � ������
function heal_self()
	local curHp, maxHp, curMp, maxMp, curCp, maxCp = l2c_getHPMPCPWeight()
	local hp_percent = 100.0 * curHp / maxHp
	local curTime = l2h_time()
	-- use heal item
	use_heal_item()
	--if heal_item_use_enable then
	--	if hp_percent <= heal_item_use_hp then
	--		if (curTime - sys_last_heal_item_use_time) > heal_item_reuse_secs then
	--			l2h_print( script_name, " Self heal: use item!\n" )
	--			l2c_useItem( heal_itemID );
	--			sys_last_heal_item_use_time = curTime
	--		end
	--	end
	--end
	if heal_self_enable then -- �������� ������ ����
		local user = l2c_getStats()
		if hp_percent < heal_battle_percent then
			l2h_print( script_name, " Self heal: need self BATTLE heal!\n" )
			myTargetObjectTimeout( user.objectID, 5 )
			myUseSkill( heal_battle_skill )
			wait_end_of_cast()
		end
		if hp_percent < heal_percent then
			l2h_print( script_name, " Self heal: need self heal!\n" )
			myTargetObjectTimeout( user.objectID, 5 )
			myUseSkill( heal_skill )
			wait_end_of_cast()
		end
	end
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


function CARDINAL_USE_BALANCE_LIFE()
	if not balance_life_enable then return end
	local party = l2c_getParty();
	if not party then return end
	local nparty = #party;
	if nparty < 1 then return end
	local i = 0;
	local good_members = 0;
	local bad_members = 0;
	local curHp, maxHp, curMp, maxMp = l2c_getHPMPCPWeight();
	local my_percent = 100.0 * curHp / maxHp;
	local my_mp_percent = 100.0 * curMp / maxMp;
	if my_mp_percent < balance_life_min_mp_limit then return end
	if my_percent >= balance_life_good_hp then good_members = good_members + 1 end
	if my_percent <= balance_life_bad_hp then bad_members = bad_members + 1 end
	for i=1, nparty do
		local percent = 100.0 * party[i].curHp / party[i].maxHp;
		if percent >= balance_life_good_hp then good_members = good_members + 1 end
		if percent <= balance_life_bad_hp then bad_members = bad_members + 1 end
	end
	--l2h_print( script_name, " Balance life check: good/bad: ", good_members, " / ", bad_members, "\n" );
	if (good_members >= balance_life_min_good_players) and
		(bad_members <= balance_life_max_bad_players) and (bad_members > 0) then
		l2h_print( script_name, " Balance life activated: good/bad: ", good_members, " / ", bad_members, "\n" );
		myUseSkill( balance_life_skill );
		wait_end_of_cast();
	end
end


function CARDINAL_USE_MAJOR_GROUP_HEAL()
	if not group_heal_enable then return end
	local party = l2c_getParty();
	if not party then return end
	local nparty = #party;
	if nparty < 1 then return end
	local i = 0;
	local bad_members = 0;
	local curHp, maxHp = l2c_getHPMPCPWeight();
	local my_percent = 100.0 * curHp / maxHp;
	if my_percent <= group_heal_bad_hp then bad_members = bad_members + 1 end
	for i=1, nparty do
		local percent = 100.0 * party[i].curHp / party[i].maxHp;
		if percent <= group_heal_bad_hp then bad_members = bad_members + 1 end
	end
	-- l2h_print( script_name, " Group heal check: bad: ", bad_members, "\n" );
	if bad_members >= group_heal_min_bad_players then
		l2h_print( script_name, " Group heal activated: bad: ", bad_members, "\n" );
		myUseSkill( group_heal_skill );
		wait_end_of_cast();
	end
end

-- ��� � ���� ���� �� ������-�������
-- ���� ��� ������������ ��������� � ����� )
function main_AI()
	if not l2c_is_INGAME() then return end
	if sys_should_exit then return end -- ���� ����� "���� ������"
	---- cardinal unique ----
	CARDINAL_USE_BALANCE_LIFE();
	CARDINAL_USE_MAJOR_GROUP_HEAL();
	---- cardinal unique ----
	check_party(); -- �������� �� ���� �� ���� �� ���� ��������
	if sys_should_exit then return end -- ���� ����� "���� ������"
	if follow_enable then follow( follow_name, follow_distance, 10 ) end -- ������� �� ��� �������
	if sys_should_exit then return end -- ���� ����� "���� ������"
	check_self_buffs(); -- �������� ���� ����
	if sys_should_exit then return end -- ���� ����� "���� ������"
	rebuff_party(); -- �������� ���� ����
	if sys_should_exit then return end -- ���� ����� "���� ������"
	heal_self(); -- ��������� ���� ����
	if sys_should_exit then return end -- ���� ����� "���� ������"
	l2h_delay( 500 ); -- ��������
end


-- ��� ������ �������� ������
l2h_print( script_name, "  script started\n" );
--l2h_registerSysStopVar( "sys_should_exit" );
-- � ���� �� ��?
if not l2c_is_INGAME() then
	l2h_print( script_name, " Not in game!\n" );
	return;
end

-- ������� �����
l2c_autoSoulshot( soulshot_itemID )
l2c_autoSoulshot( spiritshot_itemID )

-- ��� ���� �� ��������!
while not is_in_party() do l2h_delay( 1000 ); end

-- ������� ������ ���� ��������
check_self_buffs()

-- ��� ����� ������ ������ ����
follow( follow_name, heal_party_distance, 10 );
rebuff_party();

-- ����
repeat
	main_AI(); -- ������� ��
	if sys_should_exit then break end -- ���� ����� "���� ������"
	-- � ���� �� �� ��� ���?
	if not l2c_is_INGAME() then break end
until true == false -- ���� ����� ��������� ���� true �� ������ ������ false

l2h_print( script_name, " script stopped\n" )
