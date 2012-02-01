Lineage II Login server, compatible with L2J Java server

"Compatible" means:
 - supports L2J game servers
 - uses the same login server config files as L2J login server
 - uses the same mysql database as L2J login server

Problems and incompatibilities:
 - negative game server hexids not supported (remove "-" sign in front of server hexid in DB and in hexid.txt)
   (Don't know Java's BigInteger type negative number bit format or too lazy to work on it)
 - login telnet is not supported (and telnet config is ignored)
 - mmo.properties config is ignored
 - some config values in loginserver.properties about FloodProtector are ignored, and FloodProtector is not done
 - ShowLicense config option is ignored as it is violating Lineage client <=> Login server protocol.
   So, ShowLicense is always true.

Platform: Win32 >= Win2000, possibly WinXP (other untested :) )
  May require installation of Microsoft Visual C++ 2008 redistributable package
  see google: http://www.google.com/search?q=Microsoft+Visual+C%2B%2B+2008+redistributable+package