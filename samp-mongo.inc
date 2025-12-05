#if defined _samp_mongo_included
	#endinput
#endif
#define _samp_mongo_included

// Baglanti ve Yapi
native Mongo_Connect(const uri[], const database[]);
native Mongo_NewDocument();

// Veri Ekleme (Builder)
native Mongo_AddString(const key[], const value[]);
native Mongo_AddInt(const key[], value);
native Mongo_AddFloat(const key[], Float:value);

// Islem Fonksiyonlari
native Mongo_Insert(connectionID, const collection[]);
native Mongo_Find(connectionID, const collection[]);
native Mongo_Next(resultID);
native Mongo_FreeResult(resultID);
native Mongo_GetStr(resultID, const key[], dest[], size = sizeof(dest));
native Mongo_GetInt(resultID, const key[]);

// YENI: Guncelleme ve Silme
native Mongo_Update(connectionID, const collection[], const filter_key[], const filter_value[]);
native Mongo_Delete(connectionID, const collection[]);
