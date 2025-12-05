
# samp-mongo

**[EN]** A modern, high-performance, statically linked MongoDB plugin for SA-MP (San Andreas Multiplayer), written in C++17 using the official MongoDB C++ Driver.

**[TR]** SA-MP (San Andreas Multiplayer) için modern C++17 ve resmi MongoDB sürücüleri kullanılarak geliştirilmiş, yüksek performanslı ve statik linklenmiş bir MongoDB eklentisidir.

----------

## 🇺🇸 English Documentation

### Features

-   **Modern Architecture:** Built with C++17 and the official mongocxx driver.
    
-   **Static Linking:** No external DLL dependencies required (Single file samp-mongo.dll).
    
-   **Full CRUD Support:** Insert, Find, Update, and Delete operations.
    
-   **Builder Pattern:** Construct BSON documents natively in Pawn without complex string formatting.
    
-   **Connection Pooling:** Efficiently manages multiple database connections.
    

### Installation

1.  Download the latest samp-mongo.dll from the Releases page.
    
2.  Copy the file to your server's plugins/ folder.
    
3.  Copy samp-mongo.inc to your pawno/include/ folder.
    
4.  Add "plugins samp-mongo" (or .so for Linux) to your server.cfg.
    

### Usage Example (Pawn)

```
#include <samp-mongo>

new DB_Conn;

public OnGameModeInit()
{
    // 1. Connect to Database
    DB_Conn = Mongo_Connect("mongodb://localhost:27017", "GameServer");

    if(DB_Conn > 0)
    {
        // 2. Insert Data
        Mongo_NewDocument(); // Reset builder
        Mongo_AddString("Username", "Furkan");
        Mongo_AddInt("Score", 150);
        Mongo_Insert(DB_Conn, "Players");

        // 3. Find Data
        Mongo_NewDocument();
        Mongo_AddString("Username", "Furkan"); // Filter
        
        new result = Mongo_Find(DB_Conn, "Players");
        while(Mongo_Next(result))
        {
            new name[24], score;
            Mongo_GetStr(result, "Username", name);
            score = Mongo_GetInt(result, "Score");
            printf("Found: %s - Score: %d", name, score);
        }
        Mongo_FreeResult(result);
        
        // 4. Update Data
        Mongo_NewDocument();
        Mongo_AddInt("Score", 200); // New value
        // Update documents where "Username" equals "Furkan":
        Mongo_Update(DB_Conn, "Players", "Username", "Furkan");
        
        // 5. Delete Data
        Mongo_NewDocument();
        Mongo_AddString("Username", "Furkan");
        Mongo_Delete(DB_Conn, "Players");
    }
    return 1;
}

```

### Building from Source

1.  Install CMake and Visual Studio 2022.
    
2.  Install mongo-cxx-driver:x86-windows-static using vcpkg.
    
3.  Clone the repository and build:
    
    cmake --preset samp-x86-debug cmake --build --preset debug
    

----------

## 🇹🇷 Türkçe Dokümantasyon

### Özellikler

-   **Modern Mimari:** C++17 ve resmi mongocxx sürücüsü.
    
-   **Statik Linkleme:** Yanında ekstra DLL dosyaları taşımanıza gerek yoktur (Tek dosya samp-mongo.dll).
    
-   **Tam CRUD Desteği:** Veri Ekleme, Okuma, Güncelleme ve Silme işlemleri.
    
-   **Builder Yapısı:** Pawn içinde karmaşık JSON stringleri ile uğraşmanıza gerek kalmaz. Verileri fonksiyonlarla eklersiniz.
    
-   **Çoklu Bağlantı:** Aynı anda birden fazla veritabanına bağlanabilme.
    

### Kurulum

1.  Releases sayfasından güncel samp-mongo.dll dosyasını indirin.
    
2.  Dosyayı sunucunuzun plugins/ klasörüne atın.
    
3.  samp-mongo.inc dosyasını pawno/include/ klasörüne atın.
    
4.  server.cfg dosyasına "plugins samp-mongo" (Linux için .so) satırını ekleyin.
    

### Kullanım Örneği (Pawn)

```
#include <samp-mongo>

new DB_Baglanti;

public OnGameModeInit()
{
    // 1. Veritabanına Bağlan
    DB_Baglanti = Mongo_Connect("mongodb://localhost:27017", "OyunSunucusu");

    if(DB_Baglanti > 0)
    {
        // 2. Veri Ekleme (Insert)
        Mongo_NewDocument(); // Sepeti hazırla
        Mongo_AddString("Kullanici", "Furkan");
        Mongo_AddInt("Skor", 150);
        Mongo_Insert(DB_Baglanti, "Oyuncular");

        // 3. Veri Çekme (Find)
        Mongo_NewDocument();
        Mongo_AddString("Kullanici", "Furkan"); // Filtrele
        
        new sonuc = Mongo_Find(DB_Baglanti, "Oyuncular");
        while(Mongo_Next(sonuc))
        {
            new isim[24], skor;
            Mongo_GetStr(sonuc, "Kullanici", isim);
            skor = Mongo_GetInt(sonuc, "Skor");
            printf("Bulunan: %s - Skor: %d", isim, skor);
        }
        Mongo_FreeResult(sonuc);
        
        // 4. Güncelleme (Update)
        Mongo_NewDocument();
        Mongo_AddInt("Skor", 200); // Yeni değer
        // Kimi güncelleyeceğini 3. ve 4. parametrede belirt:
        Mongo_Update(DB_Baglanti, "Oyuncular", "Kullanici", "Furkan");
        
        // 5. Silme (Delete)
        Mongo_NewDocument();
        Mongo_AddString("Kullanici", "Furkan"); // Filtre
        Mongo_Delete(DB_Baglanti, "Oyuncular");
    }
    return 1;
}

```

### Derleme (Geliştiriciler İçin)

Projeyi kaynak kodundan derlemek isterseniz:

1.  CMake ve Visual Studio 2022 (C++ araçlarıyla) kurun.
    
2.  vcpkg kullanarak mongo-cxx-driver:x86-windows-static paketini kurun.
    
3.  Projeyi klonlayın ve aşağıdaki komutla derleyin:
    
    cmake --preset samp-x86-debug cmake --build --preset debug
    

## License

MIT License