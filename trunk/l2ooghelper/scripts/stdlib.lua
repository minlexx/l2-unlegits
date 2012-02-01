-- File with most standard functions used in scripts
-- include it in your script:
-- dofile( "stdlib.lua" );
--
-- Файл со стандартными функциями для большинства скриптов
-- Подключать эти функции к своему скрипту так:
-- dofile( "stdlib.lua" );

-- вычисляет дистанцию по dx, dy, dz: корень( dx*dx + dy*dy + dz*dz )
-- параметр dz может быть пропущен (не задан)
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

-- функция вернет true, если юзер в пати
function is_in_party()
	local party = l2c_getParty();
	local nMembers = #party;
	if nMembers < 1 then return false end -- no party
	return true
end

-- Функция пытается затаргетить objectID в течение timeoutSecs секунд
-- вернет true, если таргет удался, иначе false
function targetObject( objectID, timeoutSecs )
	if timeoutSecs == nil then timeoutSecs = 2 end
	local targetStartTime = l2h_time();
	local targetObjectId = 0;
	local timePassed = 0;
	targetObjectId = l2c_getTarget();
	if targetObjectId == objectID then return true end -- уже на таргете, ничо не надо делать
	l2c_action( objectID ); -- пытаемся затаргетить
	l2h_delay( 100 );
	repeat -- проверяем в цикле, взят ли объект на цель
		targetObjectId = l2c_getTarget();
		if sys_should_exit() then return end
		local curTime = l2h_time(); -- заодно смотрим по времени
		timePassed = curTime - targetStartTime;
		l2h_delay( 100 );
		if targetObjectId ~= objectID then l2c_action( objectID ); l2h_delay( 100 ); end
	until (timePassed > timeoutSecs) or (targetObjectId == objectID) -- Либо таргет удался, либо истек таймаут
	if( targetObjectId ~= objectID ) then
		return false
	end
	return true
end

-- двигаться в координаты (x,y,z), ожидая дстижения цели в течение timeout секунд
-- * tolerance - точность достижения целевых координат, абс. величина, пиксели
-- * randomness - случайное отклонение от целевых координат, пиксели
-- вернет true, если цель достигнута успешно, иначе false
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

-- выбирает на цель NPC с ником npcName;
-- вернет true если успешно, иначе false
function npcSel( npcName )
	if sys_should_exit() then return end
	local oid = l2c_getNpcObjectIdByName( npcName );
	if oid == nil then return false end
	local ret = targetObject( oid, 5 );
	if ret == false then l2h_print( "NPCSEL: cannot target [", npcName, "]!\n" ); end
	return ret
end

-- открывает диалог с НПЦ который в данный момент на таргете;
-- На таргете должен быть именно НПЦ! При необходимости функция приближается к НПЦ на расстояние 50
-- * timeoutSecs - таймаут ожидания открытия диалога с НПЦ, сек
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
	l2c_npcDlgClear(); -- очистим информацию о последнем открытом диалоге с НПЦ
	l2c_action( toid ); -- "кликнем" по НПЦ
	-- теперь ждем когда откроется диалог (NpcHtmlMessage)
	local st_time = l2h_time(); -- запомним время начала ожидания
	while true do
		l2h_delay( 50 );
		local ok = l2c_npcDlgExists(); -- существует ли диалог?)
		if ok then return true end -- да, ок
		local nowTime = l2h_time(); -- нет, тогда проверим сколько времени прошло
		if (nowTime - st_time) > timeoutSecs then
			l2h_print( "NPCDLG: ERROR: timeout waiting for NpcHtmlMessage!\n" );
			return false -- таймаут истек
		end
	end
end

-- выбирает строчку из диалога с НПЦ
-- * talk_delay_ms - сколько миллисек ждать после "клика" по ссылке
function npcDlgSel( str, talk_delay_ms )
	if talk_delay_ms == nil then talk_delay_ms = 500 end
	l2c_npcDlg( str );
	l2h_delay( talk_delay_ms );
end

-- функция закочит работу когда юзер закончит каст текущего скилла
-- ничего не возвращает
function waitEndOfCast()
	while l2c_isCastingNow() do l2h_delay( 50 ) end
end

-- использует скилл и делает задержку 100мс ля уверенности что каст скилла начался
-- чиста для удобства. величина задержки зависит от лагов канала до сервера
-- ничего не возвращает
function useSkill( skillID, delay_ms )
	if delay_ms == nil then delay_ms = 100 end
	l2c_useSkill( skillID, true );
	l2h_delay( delay_ms )
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


-- приближается к чару с ником name как минимум на расстояние distance
function follow( name, distance )
	-- получим objectID чара
	local randomness = 60;
	local objectID = l2c_getCharObjectIdByName( name );
	-- а есть ли такой чар вообще?
	if objectID == nil then
		l2h_print( "FOLLOW: Character [", name, "] is not visible!\n" );
		return
	end
	-- вычислим случайное отклонение от цели
	if randomness >= distance then randomness = distance/2 end -- отклонение никак не может быть больше расстояния до цели
	local x_rand = math.random( (0-randomness), randomness )
	local y_rand = math.random( (0-randomness), randomness )
	local x, y, z        -- наши координаты будут тут
	local gox, goy, goz  -- двигаться будем туда
	local c              -- инфо о чаре (цели) будет тут
	-- а надо ли нам следовать вообще?
	x,y,z = l2c_getPos(); -- получим наши координаты
	c = l2c_getObjectInfoByObjectId( objectID ); -- инфо о цели "преследования"
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
		if (c.xDst ~= 0x7FFFFFFF) and (c.yDst ~= 0x7FFFFFFF) then
			gox = c.xDst;
			goy = c.yDst;
			if c.zDst ~= 0x7FFFFFFF then goz = c.zDst else goz = c.z end
		else
			gox = c.x;
			goy = c.y;
			goz = c.z;
		end
		l2c_moveTo( gox + x_rand, goy + y_rand, goz ) -- двинемся к нему
		l2h_delay( 300 ); -- подождем 300 ms
		x,y,z = l2c_getPos(); -- получим снова наши координаты
		-- проверим разницу наших координат и координат чара.
		-- если разница больше дистанции следования, цикл следования повторится
	until ((math.abs(gox - x) < distance) and (math.abs(goy - y) < distance) and (math.abs(goz - z) < distance))
end
