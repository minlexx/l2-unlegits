<?php
$DP_DIR = 'Y:/bin/l2pvp/gameserver/data/stats/skills';
$skills = array();

function echo_file_err( $fn, $nl, $err )
{
	$fnb = basename( $fn );
	echo "ERROR [$fnb:$nl]: $err\n";
}

function process_file( $fn )
{
	global $skills;
	$f = fopen( $fn, 'rt' );
	if( $f === false )
	{
		echo "ERROR: cannot open [$fn]!\n";
		return;
	}
	$nl = 0;
	while( !feof( $f ) )
	{
		$s = fgets( $f, 4096 );
		if( $s === false ) continue;
		$nl++;
		$pos = stripos( $s, '<skill' );
		if( $pos === false ) continue;
		$id_pos = stripos( $s, 'id="' );
		$name_pos = stripos( $s, 'name="' );
		if( $id_pos === false || $name_pos === false )
		{
			echo_file_err( $fn, $nl, 'id_pos or name_pos are false, line=['.$s.']' );
			continue;
		}
		// get skill id
		$id_pos += 4; // 'id="'
		$id_s = '';
		while( $s[$id_pos] != '"' )
		{
			$id_s .= $s[$id_pos];
			$id_pos++;
		}
		// get skill name
		$name_pos += 6; // 'name="'
		$name_s = '';
		while( $s[$name_pos] != '"' )
		{
			$name_s .= $s[$name_pos];
			$name_pos++;
		}
		//echo $s.': id ['.$id_s.'] name ['.$name_s."]\n";
		$name = str_replace( "'", "''", $name_s );
		$skills[(int)$id_s] = $name;
	}
	fclose( $f );
}

$dir = opendir( $DP_DIR );
echo "Parsing XML files...\n";
while( ($fn = readdir( $dir )) !== false )
{
	if( $fn != '.' && $fn != '..' )
		process_file( $DP_DIR.'/'.$fn );
}
echo "Parsed ".count($skills)." skills\n";

echo "Creating SQL file...\n";
$f = fopen( 'skills_t24.sql', 'wt' );
fwrite( $f,
"CREATE TABLE `skills` (\n".
"  `id` int NOT NULL default '0',\n".
"  `name` text NOT NULL default '',\n".
"  PRIMARY KEY  (`id`)\n".
");\n\n" );
foreach( $skills as $skillId => $skillName )
{
	fwrite( $f, "INSERT INTO skills( `id`, `name` ) VALUES ( '".$skillId."', '".$skillName."' );\n" );
}
fclose( $f );
echo "Done.\n";

?>