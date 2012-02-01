l2h_console_enable( true );
l2h_print( "Hello, world!\n" );
--l2h_delay( 1000 );
--l2h_soundAlert();
l2h_print( "tm: ", l2h_time(), "; tm_msec: ", l2h_timeMsec(), "\n" );

-- tests
function tests()
	if( l2c_is_INGAME() ) then l2h_print( "IN GAME\n" ); else l2h_print( "NOT IN GAME!\n" ); end
	local curHp, maxHp, curMp, maxMp, curCp, maxCp, curLoad, maxLoad = l2c_getHPMPCPWeight();
	l2h_print( "HP: ", curHp, " / ", maxHp, "(", (100.0*curHp/maxHp), "%)", "\n" );
	l2h_print( "MP: ", curMp, " / ", maxMp, "(", (100.0*curMp/maxMp), "%)", "\n" );
	l2h_print( "CP: ", curCp, " / ", maxCp, "(", (100.0*curCp/maxCp), "%)", "\n" );
	l2h_print( "Load: ", curLoad, " / ", maxLoad, "(", (100.0*curLoad/maxLoad), "%)", "\n" );
	local userInfo = l2c_getStats();
	l2h_print( "Char: [", userInfo.name, "]  oID [", userInfo.objectID,
		"]  heading [", userInfo.heading, "]  karma [", userInfo.karma, "]\n" );
	l2h_print( "========================================================\n" );
end

--[[
	public final static int ALL = 0;
	public final static int SHOUT = 1; //!
	public final static int TELL = 2;
	public final static int PARTY = 3; //#
	public final static int CLAN = 4;  //@
	public final static int GM = 5;
	public final static int PETITION_PLAYER = 6; // used for petition
	public final static int PETITION_GM = 7; //* used for petition
	public final static int TRADE = 8; //+
	public final static int ALLIANCE = 9; //$
	public final static int ANNOUNCEMENT = 10;
	public final static int PARTYROOM_ALL = 16; //(Red)
	public final static int PARTYROOM_COMMANDER = 15; //(Yellow)
	public final static int HERO_VOICE = 17;
	public final static int BATTLEFIELD = 20;
]]--
function tests_say()
	l2c_say( 1, "WTS Sword of Valhalla" ); -- shout!
	l2h_delay( 1000 );
	l2c_sayPm( "Hello!", "negr1" ); -- whisper to negr1
end

function tests_npcDlg()
	--l2c_npcDlg( "Quest" );
	l2c_npcDlg( "Raid Monster Information" );
end

function test_move()
	local x, y, z = l2c_getPos();
	l2h_print( "Coords: ", x, ", ", y, ", ", z, "\n" );
	local isSitting, isRunning = l2c_getSitRun();
	l2h_print( "Sit, run: ", isSitting, ", ", isRunning, "\n" );
	
	--l2c_moveTo( 77194, 148629, -3592 );
	--l2h_delay( 1000 );
	--l2c_moveTo( 77394, 148629, -3592 );
	--l2h_delay( 1000 );
	--l2c_moveTo( 77194, 148629, -3592 );
	l2c_moveToDelta( -300, 0, 0 );
	l2h_delay( 2000 );
	l2c_moveToDelta( 0, 100, 0 );
	l2h_delay( 2000 );
	l2c_moveToDelta( 0, -100, 0 );
	l2h_delay( 2000 );
	l2c_moveToDelta( 300, 0, 0 );
	l2h_delay( 2000 );
end

function test_targ_atk()
	local oid, hp, mhp = l2c_getTarget();
	l2h_print( "Target info: oid, hp, maxHp: ", oid, ", ", hp, ", ", mhp, "\n" );
	l2c_action( 1209023255 );
	l2h_delay( 1000 );
	l2c_targetCancel();
	--l2c_attack();
	l2c_targetByName( "Claus" );
end

function test_items()
	local iid, oid = l2c_getPaperdollItem( 12 ); -- L2_PAPERDOLL_CHEST 12
	l2h_print( "iid, oid = ", iid, ", ", oid, "\n" );
	l2h_delay( 1000 );
	l2c_useItemByObjectId( oid );
	l2h_delay( 1000 );
	l2c_useItemByObjectId( oid );
	local heal_itemID = 1061; -- Healing potion
	l2c_useItem( heal_itemID );
	local spiritshot_itemID = 3948; -- Blessed Spiritshot: D Grade
	l2c_autoSoulshot( spiritshot_itemID, true );
	l2h_delay( 1000 );
	l2c_autoSoulshot( spiritshot_itemID, false );
end

function test_buffs()
	local buffs = l2c_getBuffs();
	local buffCount = #buffs;
	local i = 0;
	l2h_print( "===== ", buffCount, " buffs =====\n" );
	if buffCount > 0 then
		for i=1,buffCount do
			l2h_print( "   ", i, ": [", buffs[i].skillID, "] ", buffs[i].skillName, ", ", buffs[i].durationSecs, "s\n" );
		end
	end
	l2c_buffCancel( 1059 );
end

function test_skills()
	local lvl = l2c_getSkillLevel( 1204 ); -- wind walk
	l2h_print( "WW lvl: ", lvl, "\n" );
	local userInfo = l2c_getStats();
	-- target self
	l2c_action( userInfo.objectID );
	-- use wind walk
	l2c_useSkill( 1204, false );
	-- wait until cast begins
	l2h_print( "wait cast start... " );
	while not l2c_isCastingNow() do
		l2h_delay( 100 );
		l2h_print( "w" );
	end
	l2h_print( "\n" );
	local reuse = l2c_getSkillReuseLeft( 1204 );
	l2h_print( "Reuse = ", reuse, "\n" );
end

--l2h_print( "Waiting for stop...\n" );
--while( sys_should_exit() == false ) do
--	l2h_delay( 500 );
--	tests();
--	tests_say();
--end
--l2h_print( "\nWait OK!\n" );

--test_move();
--test_targ_atk();
--test_items();
--test_buffs();
test_skills();

l2h_print( "Waiting for stop...\n" );
while( sys_should_exit() == false ) do
	if l2c_isCastingNow() then
		l2h_print( "casting\n" );
	else
		l2h_print( "not casting\n" );
	end
end
l2h_print( "\nWait OK!\n" );

