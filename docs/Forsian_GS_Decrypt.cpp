// [17:13:24] <Forsian>

void Decrypt(unsigned char *Data, int size_data){
		if (!Crypt) return;
		if (Settings.DebugLogFileLineage) {
			fprintf(LogLineagePacket, "=======================================? %d (TCP: ~? %d)=============================================\nPacket:\n", NumberLineage2Packet, NumberIPPacket);
			PrintData(Data, size_data, LogLineagePacket);
		}
		int i;
		int j=0;
		int Temp;
		int Temp2 = 0; 
		for (i=0; i < size_data; i++){
			Temp = Data[i] & 0xFF;
			Data[i] = (byte)(Temp ^ (CryptKey[j++] & 0xFF) ^ Temp2);
			Temp2 = Temp;
			if (j == 16) j = 0;
		}
		if (Settings.DebugLogFileLineage) {
			fprintf(LogLineagePacket, "\nOld Key:       ");
			for (i = 0; i < 16; i++){
				fprintf(LogLineagePacket, "%02X ", CryptKey[i]);
			}
		}
		long old = 0;
		old |= CryptKey[8] & 0xFF;
		old |= CryptKey[9] << 0x08 & 0xFF00;
		old |= CryptKey[10] << 0x10 & 0xFF0000;
		old |= CryptKey[11] << 0x18 & 0xFF000000;
		if (Settings.DebugLogFileLineage) {
			fprintf(LogLineagePacket, "\nLong Old = 0x%08X\n", old);
			fprintf(LogLineagePacket, "Size: %d(dec) %04X(hex)\n", size_data, size_data);
		}
		old += size_data;
		if (Settings.DebugLogFileLineage) {
			fprintf(LogLineagePacket, "Old+Size: 0x%08X\n", old);
		}
		CryptKey[8] = (byte)(old & 0xFF);
		CryptKey[9] = (byte)(old >> 0x08 & 0xFF);
		CryptKey[10] = (byte)(old >> 0x10 & 0xFF);
		CryptKey[11] = (byte)(old >> 0x18 & 0xFF);
		if (Settings.DebugLogFileLineage) {
			fprintf(LogLineagePacket, "New Crypt Key: ");
			for (i = 0; i < 16; i++){
				fprintf(LogLineagePacket, "%02X ", CryptKey[i]);
			}
			fprintf(LogLineagePacket, "\nPacket After Decrypt:\n");
			PrintData(Data, size_data, LogLineagePacket);
			fprintf(LogLineagePacket, "New Crypt Key: ");
			for (i = 0; i < 16; i++){
				fprintf(LogLineagePacket, "%02X ", CryptKey[i]);
			}
			fprintf(LogLineagePacket, "\n");
			fflush(LogLineagePacket);
		}
	return;
}
