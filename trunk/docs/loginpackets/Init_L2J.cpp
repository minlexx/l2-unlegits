/**
 * Format: dd b dddd s
 * d: session id
 * d: protocol revision
 * b: 0x90 bytes : 0x80 bytes for the scrambled RSA public key
 *                 0x10 bytes at 0x00
 * d: unknow
 * d: unknow
 * d: unknow
 * d: unknow
 * s: blowfish key
 *
 */
 
class Init: public L2LoginServerPacket
{
private:
	int  _sessionId;
	byte *_publicKey;
	byte *_blowfishKey;

public:
	Init( L2LoginClient &client )
	{
		this( client.getScrambledModulus(), client.getBlowfishKey(), client.getSessionId() );
	}

    Init( byte *publickey, byte *blowfishkey, int sessionId )
    {
    	_sessionId = sessionId;
    	_publicKey = publickey;
    	_blowfishKey = blowfishkey;
    }

protected:
	/**
	 * @see com.l2jserver.mmocore.network.SendablePacket#write()
	 */
	@Override
	void write()
	{
		writeC(0x00); // init packet id          // 1

    	writeD(_sessionId); // session id        // 1+4=5
    	writeD(0x0000c621); // protocol revision // 5+4=9

    	writeB(_publicKey); // RSA Public Key    // 9+128=137
    	
    	// here must be unknown 16 bytes??

    	// unk GG related?
    	writeD(0x29DD954E);
    	writeD(0x77C39CFC);
    	writeD(0x97ADB620);
    	writeD(0x07BDE0F7); //                   // 137+16=153

    	writeB(_blowfishKey); // BlowFish key    // 153+16=169
        writeC(0x00); // null termination ;)     // 170? :\
	}
}
