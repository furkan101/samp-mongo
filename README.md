# samp-mongo

**[EN]** A modern, modular, and high-performance asynchronous MongoDB plugin for SA-MP (San Andreas Multiplayer), written in C++17.
**[TR]** SA-MP için modern C++17 ile yazılmış, modüler, yüksek performanslı ve asenkron çalışan MongoDB eklentisi.

---

## 🇺🇸 English Documentation

### Features
* **Modular Architecture:** Clean code structure separating Worker, Natives, and Core logic.
* **True Asynchronous:** Non-blocking database operations using a dedicated background thread.
* **Thread-Safe:** Implements Mutex locks and Task Queues to prevent server freezes or crashes.
* **BSON Builder:** Native C++ BSON construction for fast data handling.
* **Static Linking:** No external DLL dependencies required (except the plugin itself).

### Installation
1.  Download the latest `samp-mongo.dll` (Windows) or `.so` (Linux) from Releases.
2.  Copy the file to your server's `plugins/` folder.
3.  Copy `samp-mongo-v2.inc` to your `pawno/include/` folder.
4.  Add `plugins samp-mongo` to your `server.cfg`.

### Usage Example (Pawn)

```pawn
#include <a_samp>
#include <samp-mongo-v2>

new DB;

// Callback to handle results
forward OnUserDataLoaded(resultid, playerid);
public OnUserDataLoaded(resultid, playerid)
{
    // Iterate through results
    while(MG_ResultNext(resultid))
    {
        new name[24], score;
        MG_GetResultString(resultid, "Username", name);
        score = MG_GetResultInt(resultid, "Score");
        
        printf("Found User: %s | Score: %d", name, score);
    }
    
    // Always free the result memory!
    MG_FreeResult(resultid);
    return 1;
}

public OnGameModeInit()
{
    // 1. Connect
    DB = MG_Connect("mongodb://localhost:27017", "GameDB");

    // 2. Insert Data (Fire and Forget)
    MG_CreateDoc();
    MG_AddString("Username", "PlayerOne");
    MG_AddInt("Score", 1500);
    MG_QueryAsync(DB, "Users", MG_INSERT, "", -1);

    // 3. Find Data (Async with Callback)
    MG_CreateDoc();
    MG_AddString("Username", "PlayerOne"); // Filter
    
    // Trigger "OnUserDataLoaded" when finished
    MG_QueryAsync(DB, "Users", MG_FIND, "OnUserDataLoaded", 0);

    return 1;
}
```

### Build Instructions (For Developers)

Requirements:

-   Visual Studio 2022 (C++ Desktop Development)
    
-   CMake 3.15+
    
-   **vcpkg** (for dependencies)
    

1.  Install dependencies:
    
    Bash
    
    ```
    vcpkg install mongo-cxx-driver:x86-windows-static
    
    ```
    
2.  Configure and Build:
    
    Bash
    
    ```
    cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DCMAKE_TOOLCHAIN_FILE=[PATH_TO_VCPKG]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static
    cmake --build build --config Release
    
    ```
    

----------

## 🇹🇷 Türkçe Dokümantasyon

### Özellikler

-   **Modüler Mimari:** İşçi (Worker), Native ve Çekirdek (Core) mantığı ayrılmış temiz yapı.
    
-   **Tam Asenkron:** Arka plan iş parçacığı (Thread) sayesinde sunucuyu asla dondurmaz (Lag yapmaz).
    
-   **Thread-Safe:** Çökme ve veri kaybını önleyen Mutex ve Kuyruk sistemi.
    
-   **Statik Linkleme:** Ekstra DLL dosyalarına ihtiyaç duymaz, tek dosya çalışır.
    

### Kurulum

1.  Releases kısmından güncel `samp-mongo.dll` dosyasını indirin.
    
2.  Dosyayı sunucunuzun `plugins/` klasörüne atın.
    
3.  `samp-mongo-v2.inc` dosyasını `pawno/include/` klasörüne kopyalayın.
    
4.  `server.cfg` dosyasına `plugins samp-mongo` satırını ekleyin.
    

### Kullanım Örneği (Pawn)

Kod snippet'i

```
#include <a_samp>
#include <samp-mongo-v2>

new DB_Baglanti;

// Veri geldiğinde çalışacak fonksiyon (Callback)
forward OyuncuVerisiGeldi(sonucid, oyuncuid);
public OyuncuVerisiGeldi(sonucid, oyuncuid)
{
    // Gelen sonuçlar arasında dön
    while(MG_ResultNext(sonucid))
    {
        new isim[24], skor;
        MG_GetResultString(sonucid, "KullaniciAdi", isim);
        skor = MG_GetResultInt(sonucid, "Skor");
        
        printf("Bulunan Oyuncu: %s | Skor: %d", isim, skor);
    }
    
    // Hafızayı temizlemeyi unutmayın!
    MG_FreeResult(sonucid);
    return 1;
}

public OnGameModeInit()
{
    // 1. Bağlan
    DB_Baglanti = MG_Connect("mongodb://localhost:27017", "OyunSunucusu");

    // 2. Veri Ekle (Cevap beklemeye gerek yok)
    MG_CreateDoc();
    MG_AddString("KullaniciAdi", "Ahmet");
    MG_AddInt("Skor", 500);
    MG_QueryAsync(DB_Baglanti, "Oyuncular", MG_INSERT, "", -1);

    // 3. Veri Çek (Callback ile)
    MG_CreateDoc();
    MG_AddString("KullaniciAdi", "Ahmet"); // Filtrele
    
    // İşlem bitince "OyuncuVerisiGeldi" fonksiyonunu çağır
    MG_QueryAsync(DB_Baglanti, "Oyuncular", MG_FIND, "OyuncuVerisiGeldi", 0);

    return 1;
}

```

### Kaynak Koddan Derleme

Gereksinimler:

-   Visual Studio 2022
    
-   CMake 3.15+
    
-   **vcpkg** (Kütüphaneler için)
    

1.  Kütüphaneleri kurun:
    
    Bash
    
    ```
    vcpkg install mongo-cxx-driver:x86-windows-static
    
    ```
    
2.  Yapılandır ve Derle:
    
    Bash
    
    ```
    cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DCMAKE_TOOLCHAIN_FILE=[VCPKG_YOLU]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static
    cmake --build build --config Release
    
    ```
    

## License

MIT License