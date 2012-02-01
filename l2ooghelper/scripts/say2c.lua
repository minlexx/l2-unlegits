-- Демонстрация чат-хендлера
-- главная программа крутится в бесконечном цикле, рисуя точку раз в секунду,
-- как бы сигнализируя о том что основной цикл живой;
-- в это вреемя (по идее параллельно основному циклу),
-- при получении чат сообщения должна вызываться функция onChat_handler(),
-- имеющая доступ к глобальным переменным всей программы (тестируем на переменной num_calls)

-- реально на время вызова onChat основной поток ставится на паузу,
-- иначе возможны дикие проблемы синхронизации! вплоть до вылета с критом ООГ

-- по всем идеям хендлер onChat должен максимально БЫСТРО отработать
-- дабы не замедлять оснвной цикл АИ - типа что-то поменять в глобальных переменных
-- или другое, быстрое действие

num_calls = 0; -- сколько раз был вызван onChat_handler()

function onChat_handler( senderID, channelID, msg, from )
	num_calls = num_calls + 1;
	l2h_print( "onChat: OID [", senderID, "] ", channelID, ": [", msg, "] from [", from, "]\n" );
end

l2h_print( "onChat test started!\n" );
-- зарегаемся в ScriptEngine чтобы onChat_handler() вызывалась при каждом чат-сообщении
sys_register_onChat( "onChat_handler" );

while( true ) do
	l2h_print( ".\n" );
	if( sys_should_exit() ) then break end -- выход из бесконечного цикла когда нажали "стоп скрипт"
	l2h_delay( 1000 );
end

l2h_print( "onChat was called ", num_calls, " times.\n" );
l2h_print( "onChat test end.\n" );
