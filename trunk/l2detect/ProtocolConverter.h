#ifndef H_PROTOCONV
#define H_PROTOCONV

void ProtoConv_Init();
int  ProtoConv_IsEnabled();
int  ProtoConv_ConvExistsForPacket( unsigned char pcode );

bool ProtoConv_ConvertPacket( unsigned char *bytes, unsigned int len, 
							 unsigned char **newBytes, unsigned int *newLen );

#endif
