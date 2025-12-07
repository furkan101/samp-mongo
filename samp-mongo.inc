#if defined _samp_mongo_included
	#endinput
#endif
#define _samp_mongo_included

#define MG_INSERT 0
#define MG_DELETE 2
#define MG_FIND   3

native MG_Connect(const uri[], const database[]);
native MG_CreateDoc();
native MG_AddString(const key[], const value[]);
native MG_AddInt(const key[], value);
native MG_QueryAsync(connID, const collection[], type, const callback[] = "", playerid = -1);
native MG_UpdateAsync(connID, const collection[], const filterKey[], const filterVal[], const callback[] = "", playerid = -1);
native MG_ResultNext(resultID);
native MG_GetResultString(resultID, const key[], dest[], size = sizeof(dest));
native MG_GetResultInt(resultID, const key[]);
native MG_FreeResult(resultID);