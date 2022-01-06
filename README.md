Lütfen çalıştırmak için

 gcc HW3.c -o hw3
./hw3 File1 File2

Komutlarını girin. File1 process dosyası File2 ise reference string dosyası olarak girilmelidir.
Projede PagingTable'ı 2dimensional bir arrayde ilk dimension FrameNo ikinci ise tek bitlik bir valid-invalid biti olarak belirledim.
TLB ise 2dimensional bir arrayde ilk dimension PageNo ikinci ise tek FrameNo olarak belirledim. PageTable'ı RealMemory'nin 1/10'u olarak belirledim.
PageNumarası ne ise RealMemoryde'de aynı Numaralı Frame'e eşleme yaptım. 1-bit second chance replacement algoritmasındaki chance'leri tutmak için
2 Dimensional bir PTQueue (Page Table Queue) tuttum ve her page için chance bitini bu queue'dan takip ettim.
Projede komut satırları ile adım adım kodu anlattım.

Ahmet Selim Pehlivan
181101033