<?php

function qq( $s )
{
	$ss = '';
	for( $i=0; $i<strlen($s); $i++ )
	{
		if( $s[$i] == "'" ) $ss .= "'";
		$ss .= $s[$i];
	}
	return $ss;
}

mysql_connect( 'localhost', 'root', '' );
mysql_query( 'USE l2jdb' );

/*$f = fopen( 'l2detect_items.sql', 'wt' );
fwrite( $f,
"CREATE TABLE `items` (
  `id` int NOT NULL default '0',
  `name` text NOT NULL default '',
  `pic` text NOT NULL default '',
  PRIMARY KEY  (`id`)
);\n" );

$res = mysql_query( 'SELECT * FROM etcitem ORDER BY item_id' );
$i = 0;
while( ($row = mysql_fetch_assoc( $res )) !== false )
{
	fwrite( $f, "INSERT INTO items( id, name, pic ) VALUES ( '".$row['item_id']."', '".qq($row['name'])."', 'none' );\n" );
	$i++;
}
fwrite( $f, "\n" );
echo "$i items\n";


$res = mysql_query( 'SELECT * FROM armor ORDER BY item_id' );
$i = 0;
while( ($row = mysql_fetch_assoc( $res )) !== false )
{
	fwrite( $f, "INSERT INTO items( id, name, pic ) VALUES ( '".$row['item_id']."', '".qq($row['name'])."', 'none' );\n" );
	$i++;
}
fwrite( $f, "\n" );
echo "$i armors\n";


$res = mysql_query( 'SELECT * FROM weapon ORDER BY item_id' );
$i = 0;
while( ($row = mysql_fetch_assoc( $res )) !== false )
{
	fwrite( $f, "INSERT INTO items( id, name, pic ) VALUES ( '".$row['item_id']."', '".qq($row['name'])."', 'none' );\n" );
	$i++;
}
fwrite( $f, "\n" );
echo "$i weapons\n";

fclose( $f );
*/


$f = fopen( 'l2detect_mobs.sql', 'wt' );
fwrite( $f,
"CREATE TABLE `mobs` (
  `id` int NOT NULL default '0',
  `name` text NOT NULL default '',
  `title` text NOT NULL default '',
  `level` int NOT NULL default '0',
  PRIMARY KEY  (`id`)
);\n" );

$res = mysql_query( 'SELECT * FROM npc ORDER BY idTemplate' );
$i = 0;
while( ($row = mysql_fetch_assoc( $res )) !== false )
{
	fwrite( $f, "INSERT INTO mobs( `id`, `name`, `title`, `level` ) ".
		"VALUES ( '".$row['idTemplate']."', '".qq($row['name'])."', '".qq($row['title'])."', '".$row['level']."' );\n" );
	$i++;
}
fwrite( $f, "\n" );
echo "$i NPCs\n";
fclose( $f );


/*$f = fopen( 'l2detect_skills.sql', 'wt' );
fwrite( $f,
"CREATE TABLE `skills` (
  `id` int NOT NULL default '0',
  `name` text NOT NULL default '',
  PRIMARY KEY  (`id`)
);\n" );

$res = mysql_query( 'SELECT DISTINCT(skill_id), name FROM `skill_trees` ORDER BY skill_id' );
$i = 0;
while( ($row = mysql_fetch_assoc( $res )) !== false )
{
	fwrite( $f, "INSERT INTO skills( `id`, `name` ) VALUES ( '".$row['skill_id']."', '".qq($row['name'])."' );\n" );
	$i++;
}
fwrite( $f, "\n" );
echo "$i skills\n";
fclose( $f );*/

?>