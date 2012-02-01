-- ������������ ���-��������
-- ������� ��������� �������� � ����������� �����, ����� ����� ��� � �������,
-- ��� �� ������������ � ��� ��� �������� ���� �����;
-- � ��� ������ (�� ���� ����������� ��������� �����),
-- ��� ��������� ��� ��������� ������ ���������� ������� onChat_handler(),
-- ������� ������ � ���������� ���������� ���� ��������� (��������� �� ���������� num_calls)

-- ������� �� ����� ������ onChat �������� ����� �������� �� �����,
-- ����� �������� ����� �������� �������������! ������ �� ������ � ������ ���

-- �� ���� ����� ������� onChat ������ ����������� ������ ����������
-- ���� �� ��������� ������� ���� �� - ���� ���-�� �������� � ���������� ����������
-- ��� ������, ������� ��������

num_calls = 0; -- ������� ��� ��� ������ onChat_handler()

function onChat_handler( senderID, channelID, msg, from )
	num_calls = num_calls + 1;
	l2h_print( "onChat: OID [", senderID, "] ", channelID, ": [", msg, "] from [", from, "]\n" );
end

l2h_print( "onChat test started!\n" );
-- ���������� � ScriptEngine ����� onChat_handler() ���������� ��� ������ ���-���������
sys_register_onChat( "onChat_handler" );

while( true ) do
	l2h_print( ".\n" );
	if( sys_should_exit() ) then break end -- ����� �� ������������ ����� ����� ������ "���� ������"
	l2h_delay( 1000 );
end

l2h_print( "onChat was called ", num_calls, " times.\n" );
l2h_print( "onChat test end.\n" );
