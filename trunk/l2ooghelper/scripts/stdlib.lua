-- File with most standard functions used in scripts
-- include it in your script:
-- dofile( "stdlib.lua" );
--
-- ���� �� ������������ ��������� ��� ����������� ��������
-- ���������� ��� ������� � ������ ������� ���:
-- dofile( "stdlib.lua" );

-- ��������� ��������� �� dx, dy, dz: ������( dx*dx + dy*dy + dz*dz )
-- �������� dz ����� ���� �������� (�� �����)
function calc_dist( dx, dy, dz )
	if dz == nil then
		return math.sqrt( dx*dx + dy*dy )
	else
		return math.sqrt( dx*dx + dy*dy + dz*dz )
	end
end

function ensurestand()
	local isSitting, isRunnig = l2c_getSitRun()
	if isSitting then
		l2c_sitStand();
		l2h_delay( 1000 );
	end
end

function ensuresit()
	local isSitting, isRunnig = l2c_getSitRun()
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

-- ������� ������ true, ���� ���� � ����
function is_in_party()
	local party = l2c_getParty();
	local nMembers = #party;
	if nMembers < 1 then return false end -- no party
	return true
end

-- ������� �������� ����������� objectID � ������� timeoutSecs ������
-- ������ true, ���� ������ ������, ����� false
function targetObject( objectID, timeoutSecs )
	if timeoutSecs == nil then timeoutSecs = 2 end
	local targetStartTime = l2h_time();
	local targetObjectId = 0;
	local timePassed = 0;
	targetObjectId = l2c_getTarget();
	if targetObjectId == objectID then return true end -- ��� �� �������, ���� �� ���� ������
	l2c_action( objectID ); -- �������� �����������
	l2h_delay( 100 );
	repeat -- ��������� � �����, ���� �� ������ �� ����
		targetObjectId = l2c_getTarget();
		if sys_should_exit() then return end
		local curTime = l2h_time(); -- ������ ������� �� �������
		timePassed = curTime - targetStartTime;
		l2h_delay( 100 );
		if targetObjectId ~= objectID then l2c_action( objectID ); l2h_delay( 100 ); end
	until (timePassed > timeoutSecs) or (targetObjectId == objectID) -- ���� ������ ������, ���� ����� �������
	if( targetObjectId ~= objectID ) then
		return false
	end
	return true
end

-- ��������� � ���������� (x,y,z), ������ ��������� ���� � ������� timeout ������
-- * tolerance - �������� ���������� ������� ���������, ���. ��������, �������
-- * randomness - ��������� ���������� �� ������� ���������, �������
-- ������ true, ���� ���� ���������� �������, ����� false
function moveTo( x, y, z, timeout, tolerance, randomness )
	if timeout == nil then timeout = 30 end
	if tolerance == nil then tolerance = 50 end
	if randomness == nil then randomness = 0 end
	local start_time = l2h_time();
	repeat
		if sys_should_exit() then return false end -- stop script
		local cx, cy, cz = l2c_getPos(); -- get cur pos
		-- target pos reached?
		if (math.abs(cx - x) <= tolerance) and (math.abs(cy - y) <= tolerance) and (math.abs(cz - z) <= tolerance) then
			return true
		end
		-- target not reached, try to move there
		-- random XY offset (not exact move)
		local x_rand = math.random( (0-randomness), randomness );
		local y_rand = math.random( (0-randomness), randomness );
		l2c_moveTo( x + x_rand, y + y_rand, z );
		--- timeout expired?
		local cur_time = l2h_time();
		if (cur_time - start_time) > timeout then
			l2h_print( "MOVETO: (",x,",",y,",",z,") ERROR: move timeout!\n" );
			return false
		end
		l2h_delay( 300 ); -- delay 300 ms
	until false
	return false
end

-- �������� �� ���� NPC � ����� npcName;
-- ������ true ���� �������, ����� false
function npcSel( npcName )
	if sys_should_exit() then return end
	local oid = l2c_getNpcObjectIdByName( npcName );
	if oid == nil then return false end
	local ret = targetObject( oid, 5 );
	if ret == false then l2h_print( "NPCSEL: cannot target [", npcName, "]!\n" ); end
	return ret
end

-- ��������� ������ � ��� ������� � ������ ������ �� �������;
-- �� ������� ������ ���� ������ ���! ��� ������������� ������� ������������ � ��� �� ���������� 50
-- * timeoutSecs - ������� �������� �������� ������� � ���, ���
function npcDlg( timeoutSecs )
	if sys_should_exit() then return end
	if timeoutSecs == nil then timeoutSecs = 5 end
	local toid = l2c_getTarget();
	if toid == 0 then -- no target :(
		l2h_print( "NPCDLG: ERROR: No targeted NPC?\n" );
		return false
	end
	local info = l2c_getObjectInfoByObjectId( toid );
	if info == nil then
		l2h_print( "NPCDLG: cannot get info about objectID [", toid, "]! Target disappeared??\n" );
		return false
	end
	if info.type ~= "npc" then
		l2h_print( "NPCDLG: ERROR: target [", toid, "] is not npc!..\n" );
		return false
	end
	moveTo( info.x, info.y, info.z, 20, 50, 20 );
	l2c_npcDlgClear(); -- ������� ���������� � ��������� �������� ������� � ���
	l2c_action( toid ); -- "�������" �� ���
	-- ������ ���� ����� ��������� ������ (NpcHtmlMessage)
	local st_time = l2h_time(); -- �������� ����� ������ ��������
	while true do
		l2h_delay( 50 );
		local ok = l2c_npcDlgExists(); -- ���������� �� ������?)
		if ok then return true end -- ��, ��
		local nowTime = l2h_time(); -- ���, ����� �������� ������� ������� ������
		if (nowTime - st_time) > timeoutSecs then
			l2h_print( "NPCDLG: ERROR: timeout waiting for NpcHtmlMessage!\n" );
			return false -- ������� �����
		end
	end
end

-- �������� ������� �� ������� � ���
-- * talk_delay_ms - ������� �������� ����� ����� "�����" �� ������
function npcDlgSel( str, talk_delay_ms )
	if talk_delay_ms == nil then talk_delay_ms = 500 end
	l2c_npcDlg( str );
	l2h_delay( talk_delay_ms );
end

-- ������� ������� ������ ����� ���� �������� ���� �������� ������
-- ������ �� ����������
function waitEndOfCast()
	while l2c_isCastingNow() do l2h_delay( 50 ) end
end

-- ���������� ����� � ������ �������� 100�� �� ����������� ��� ���� ������ �������
-- ����� ��� ��������. �������� �������� ������� �� ����� ������ �� �������
-- ������ �� ����������
function useSkill( skillID, delay_ms )
	if delay_ms == nil then delay_ms = 100 end
	l2c_useSkill( skillID, true );
	l2h_delay( delay_ms )
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


-- ������������ � ���� � ����� name ��� ������� �� ���������� distance
function follow( name, distance )
	-- ������� objectID ����
	local randomness = 60;
	local objectID = l2c_getCharObjectIdByName( name );
	-- � ���� �� ����� ��� ������?
	if objectID == nil then
		l2h_print( "FOLLOW: Character [", name, "] is not visible!\n" );
		return
	end
	-- �������� ��������� ���������� �� ����
	if randomness >= distance then randomness = distance/2 end -- ���������� ����� �� ����� ���� ������ ���������� �� ����
	local x_rand = math.random( (0-randomness), randomness )
	local y_rand = math.random( (0-randomness), randomness )
	local x, y, z        -- ���� ���������� ����� ���
	local gox, goy, goz  -- ��������� ����� ����
	local c              -- ���� � ���� (����) ����� ���
	-- � ���� �� ��� ��������� ������?
	x,y,z = l2c_getPos(); -- ������� ���� ����������
	c = l2c_getObjectInfoByObjectId( objectID ); -- ���� � ���� "�������������"
	if c == nil then
		l2h_print( "FOLLOW: char [", name, "] suddenly disappeared!\n" );
		return false
	end
	if (c.xDst ~= 0x7FFFFFFF) and (c.yDst ~= 0x7FFFFFFF) then
		gox = c.xDst;
		goy = c.yDst;
		if c.zDst ~= 0x7FFFFFFF then goz = c.zDst else goz = c.z end
	else
		gox = c.x;
		goy = c.y;
		goz = c.z;
	end
	if (math.abs(gox - x) > distance) or (math.abs(goy - y) > distance) then
		l2c_moveTo( gox + x_rand, goy + y_rand, goz ) -- �������� � ����		
	else
		return -- �� �� � ��� �������� �� ����
	end
	-- ���� ����������
	repeat
		if sys_should_exit() then return end -- ���� ����� "���� ������"
		-- ������� ���� � ����
		c = l2c_getObjectInfoByObjectId( objectID )
		--l2h_print( "Following [", name, "] to (", c.x, ",", c.y, ",", c.z, ")\n" )
		if (c.xDst ~= 0x7FFFFFFF) and (c.yDst ~= 0x7FFFFFFF) then
			gox = c.xDst;
			goy = c.yDst;
			if c.zDst ~= 0x7FFFFFFF then goz = c.zDst else goz = c.z end
		else
			gox = c.x;
			goy = c.y;
			goz = c.z;
		end
		l2c_moveTo( gox + x_rand, goy + y_rand, goz ) -- �������� � ����
		l2h_delay( 300 ); -- �������� 300 ms
		x,y,z = l2c_getPos(); -- ������� ����� ���� ����������
		-- �������� ������� ����� ��������� � ��������� ����.
		-- ���� ������� ������ ��������� ����������, ���� ���������� ����������
	until ((math.abs(gox - x) < distance) and (math.abs(goy - y) < distance) and (math.abs(goz - z) < distance))
end
