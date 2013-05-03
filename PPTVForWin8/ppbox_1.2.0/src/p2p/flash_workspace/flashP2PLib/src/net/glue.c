#include "AS3.h"
AS3_Val gg_lib = NULL;

static AS3_Val thunk_PacketToByteArray(void *gg_clientData, AS3_Val gg_args) {
	AS3_Val bytes;
	AS3_Val packet;
	AS3_ArrayValue(gg_args, "AS3ValType, AS3ValType",  &bytes,  &packet);
	AS3_Val actionVal = AS3_GetS(packet, "action");
	AS3_Val transactionIdVal = AS3_GetS(packet, "transactionId");
	AS3_Val protocolVersionVal = AS3_GetS(packet, "protocolVersion");
	AS3_Release(AS3_CallTS("writeByte", bytes, "IntType", AS3_IntValue(actionVal)));
	AS3_Release(AS3_CallTS("writeUnsignedInt", bytes, "IntType", AS3_IntValue(transactionIdVal)));
	AS3_Release(AS3_CallTS("writeShort", bytes, "IntType", AS3_IntValue(protocolVersionVal)));
	AS3_Release(actionVal);
	AS3_Release(transactionIdVal);
	AS3_Release(protocolVersionVal);
	AS3_Release(bytes);
	AS3_Release(packet);
	return NULL;
}

static AS3_Val thunk_CreatePacketFromByteArray(void *gg_clientData, AS3_Val gg_args) {
	AS3_Val bytes;
	AS3_ArrayValue(gg_args, "AS3ValType",  &bytes);
	AS3_Val actionVal = AS3_CallTS("readUnsignedByte", bytes, "");
	AS3_Val transactionIdVal = AS3_CallTS("readUnsignedInt", bytes, "");	
	AS3_Val protocolVersionVal = AS3_CallTS("readUnsignedShort", bytes, "");
	
	AS3_Val pplive_protocol_namespace = AS3_String("com.pplive.p2p.network.protocol");
	AS3_Val PacketClass = AS3_NSGetS(pplive_protocol_namespace, "Packet");
	AS3_Val paramArray = AS3_Array("IntType, IntType, IntType",  AS3_IntValue(actionVal), AS3_IntValue(transactionIdVal), AS3_IntValue(protocolVersionVal));
	AS3_Val packet = AS3_New(PacketClass, paramArray);
	
	AS3_Release(paramArray);
	AS3_Release(PacketClass);
	AS3_Release(pplive_protocol_namespace);
	AS3_Release(protocolVersionVal);
	AS3_Release(transactionIdVal);
	AS3_Release(actionVal);
	AS3_Release(bytes);
	
	return packet;
}

static AS3_Val thunk_InitAnnounceResponsePacket(void *gg_clientData, AS3_Val gg_args) {
	AS3_Val packet;
	AS3_Val bytes;
	AS3_ArrayValue(gg_args, "AS3ValType, AS3ValType",  &packet,  &bytes);
	
	AS3_Val paramArray = AS3_Array("AS3ValType", bytes);
	AS3_Val p2p_struct_namespace = AS3_String("com.pplive.p2p.struct");
	AS3_Val PeerDownloadInfoClass = AS3_NSGetS(p2p_struct_namespace, "PeerDownloadInfo");
	AS3_Val BlockMapClass = AS3_NSGetS(p2p_struct_namespace, "BlockMap");
	AS3_Val peerDownloadInfo = AS3_New(PeerDownloadInfoClass, paramArray);	
	AS3_Val blockMapInfo = AS3_New(BlockMapClass, paramArray);
	AS3_SetS(packet, "peerDownloadInfo", peerDownloadInfo);
	AS3_SetS(packet, "blockMap", blockMapInfo);
	
	AS3_Release(blockMapInfo);
	AS3_Release(peerDownloadInfo);
	AS3_Release(BlockMapClass);
	AS3_Release(PeerDownloadInfoClass);
	AS3_Release(p2p_struct_namespace);
	AS3_Release(paramArray);
	AS3_Release(bytes);
	AS3_Release(packet);
	return NULL;
}

static AS3_Val thunk_InitPeerDownloadInfo(void *gg_clientData, AS3_Val gg_args) {
	AS3_Val info;
	AS3_Val bytes;
	AS3_ArrayValue(gg_args, "AS3ValType, AS3ValType",  &info,  &bytes);	
	AS3_Val isDownloadingVal = AS3_CallTS("readUnsignedByte", bytes, "");
	
	if (AS3_IntValue(isDownloadingVal) == 1)
	{
		AS3_Val true = AS3_True();
		AS3_SetS(info, "isDownloading", true);
		AS3_Release(true);
	}
	else
	{
		AS3_Val false = AS3_False();
		AS3_SetS(info, "isDownloading", false);
		AS3_Release(false);
	}
	
	AS3_Val onLineTimeVal = AS3_CallTS("readUnsignedInt", bytes, "");
	AS3_Val avgDownloadVal = AS3_CallTS("readUnsignedShort", bytes, "");
	AS3_Val nowDownloadVal = AS3_CallTS("readUnsignedShort", bytes, "");
	AS3_Val avgUploadVal = AS3_CallTS("readUnsignedShort", bytes, "");
	AS3_Val nowUploadVal = AS3_CallTS("readUnsignedShort", bytes, "");
	AS3_SetS(info, "onLineTime", onLineTimeVal);
	AS3_SetS(info, "avgDownload", avgDownloadVal);
	AS3_SetS(info, "nowDownload", nowDownloadVal);
	AS3_SetS(info, "avgUpload", avgUploadVal);
	AS3_SetS(info, "nowUpload", nowUploadVal);
	AS3_ByteArray_seek(bytes, 3, 1);	// ignore 3 byte reserved bytes	
	
	AS3_Release(isDownloadingVal);
	AS3_Release(onLineTimeVal);
	AS3_Release(avgDownloadVal);
	AS3_Release(nowDownloadVal);
	AS3_Release(avgUploadVal);
	AS3_Release(nowUploadVal);	
	AS3_Release(bytes);
	AS3_Release(info);
	return NULL;
}

static AS3_Val thunk_InitBlockMap(void *gg_clientData, AS3_Val gg_args) {
	AS3_Val map;
	AS3_Val bytes;
	AS3_ArrayValue(gg_args, "AS3ValType, AS3ValType",  &map,  &bytes);
	AS3_Val blockCountVal = AS3_CallTS("readUnsignedInt", bytes, "");
	AS3_Val bitset = AS3_GetS(map, "bitset");
	
	AS3_SetS(map, "blockCount", blockCountVal);
	AS3_Release(AS3_CallTS("readBytes", bytes, "AS3ValType, IntType, IntType", bitset, 0, (int)((AS3_IntValue(blockCountVal) + 7) / 8)));
	
	AS3_Release(blockCountVal);
	AS3_Release(bitset);
	AS3_Release(map);
	AS3_Release(bytes);
	return NULL;
}

static AS3_Val thunk_InitSubPiecePacket(void *gg_clientData, AS3_Val gg_args) {
	AS3_Val packet;
	AS3_Val bytes;
	AS3_ArrayValue(gg_args, "AS3ValType, AS3ValType",  &packet,  &bytes);
	AS3_Val subpiece = AS3_GetS(packet, "subpiece");
	AS3_Val blockIndexVal = AS3_CallTS("readUnsignedShort", bytes, "");
	AS3_Val subPieceIndexVal = AS3_CallTS("readUnsignedShort", bytes, "");
	
	AS3_SetS(subpiece, "blockIndex", blockIndexVal);
	AS3_SetS(subpiece, "subPieceIndex", subPieceIndexVal);
	
	AS3_Release(subpiece);
	AS3_Release(blockIndexVal);
	AS3_Release(subPieceIndexVal);
	AS3_Release(packet);
	AS3_Release(bytes);
	return NULL;
}

static AS3_Val thunk_SubPieceRequestPacketToByteArray(void *gg_clientData, AS3_Val gg_args) {
	AS3_Val packet;
	AS3_Val bytes;
	AS3_ArrayValue(gg_args, "AS3ValType, AS3ValType",  &packet,  &bytes);
	AS3_Val subpieces = AS3_GetS(packet, "subpieces");
	AS3_Val lengthVal = AS3_GetS(subpieces, "length");
	AS3_Val priorityVal = AS3_GetS(packet, "priority");
	
	AS3_Release(AS3_CallTS("writeShort", bytes, "IntType", AS3_IntValue(lengthVal)));
	AS3_Release(AS3_CallTS("WriteSubpieces", packet, "AS3ValType", bytes));
	AS3_Release(AS3_CallTS("writeShort", bytes, "IntType", AS3_IntValue(priorityVal)));
	
	AS3_Release(subpieces);
	AS3_Release(lengthVal);
	AS3_Release(priorityVal);
	AS3_Release(packet);
	AS3_Release(bytes);
	return NULL;
}

void gg_reg(AS3_Val lib, const char *name, AS3_ThunkProc p) {
	AS3_Val fun = AS3_Function(NULL, p);
	AS3_SetS(lib, name, fun);
	AS3_Release(fun);
}

int main(int argc, char **argv) {
#if defined(GGINIT_DEFINED)
	ggInit();
#endif
	gg_lib = AS3_Object("");
	gg_reg(gg_lib, "PacketToByteArray", thunk_PacketToByteArray);
	gg_reg(gg_lib, "CreatePacketFromByteArray", thunk_CreatePacketFromByteArray);
	gg_reg(gg_lib, "InitAnnounceResponsePacket", thunk_InitAnnounceResponsePacket);
	gg_reg(gg_lib, "InitPeerDownloadInfo", thunk_InitPeerDownloadInfo);
	gg_reg(gg_lib, "InitBlockMap", thunk_InitBlockMap);
	gg_reg(gg_lib, "InitSubPiecePacket", thunk_InitSubPiecePacket);
	gg_reg(gg_lib, "SubPieceRequestPacketToByteArray", thunk_SubPieceRequestPacketToByteArray);
	AS3_LibInit(gg_lib);
	return 1;
}
