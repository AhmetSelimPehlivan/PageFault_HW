#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#define Frame_Number N/10

int N=0;
int toplam_hit=0;
int toplam_miss=0;
int toplam_page_fault=0;

int getFrameInPage(int page_table[Frame_Number][2], int index);
int TLBde_var_mi(int TLB[10][2],int index){
    for(int i=0;i<10;i++){
        if(TLB[i][0]==index)
            return 1;
        }
    return 0;
}

int PTde_var_mi(int page_table[Frame_Number][2],int index){
    for (int i = 0; i < Frame_Number; i++){
        if(page_table[i][1]==1 && page_table[i][0]==index)  // eger frame valid ve page table icinde bulunduysa fault yok
            return 1;
        }
    return 0;
}
void TLBQueueUpdate(int TLBQueue[10]){
    for (int i = 0; i < 9; i++){
        TLBQueue[i]=TLBQueue[i+1];
    }
}

void PTQueueUpdate(int PTQueue[Frame_Number][2],int remove_index){
    for (int i = remove_index; i < Frame_Number; i++){ // Cikan indexden itibaren kaydir
        PTQueue[i][0]=PTQueue[i+1][0];
        PTQueue[i][1]=PTQueue[i+1][1];
    }
}

void PTQueueGiveSecondChance(int PTQueue[Frame_Number][2],int index){
    for (int i = 0; i < Frame_Number; i++) // Queue icinde page'i bul ve degerini 1 yap
        if (PTQueue[i][0] == index){
            PTQueue[i][1] = 1;
            break;
        }
}

void PTQueueLoseSecondChance(int PTQueue[Frame_Number][2]){
    for (int i = 0; i < Frame_Number; i++) // Queue icinde tum pagelerin degerini 0 yap
        if (PTQueue[i][1] == 1)
            PTQueue[i][1] = 0; 
}
int getFrameInPage(int page_table[Frame_Number][2], int index){
    int i=0;
    for (i = 0; i < Frame_Number; i++)
    {
        if (page_table[i][0]==index)
          break;
    }
    return page_table[i][0];
}
void PTye_yerlestir(int page_table[Frame_Number][2],int index,int PTQueue[Frame_Number][2]){
    int flag=0;
    int flag2=0;
    for (int i = 0; i < Frame_Number; i++)
        if(page_table[i][1]==0){
            flag = i;  //ptde ilk bos yer
            flag2=1;
            break;
        }
    if(flag2>0) {//ptde yer var
        page_table[flag][0]=index;
        page_table[flag][1]=1;
        for (int i = 0; i == Frame_Number; i++)
            if(PTQueue[i][0]==-1){
                PTQueue[i][0]=index;
                PTQueue[i][1]=0;
                break;
            }
    }
    else { //ptde yer yoksa ilk olarak Page Table Queue icindeki ilk bite bakilir
        int remove_index = 0;
        for (int i = 0; i<Frame_Number; i++)
            if (PTQueue[i][1] == 0)
            {
                remove_index = i;
                break;
            }
        PTQueueUpdate(PTQueue,remove_index);
        PTQueueUpdate(page_table,remove_index);
        page_table[Frame_Number-1][0]=index; // Yeni page her zaman en son indexe eklenecek
        page_table[Frame_Number-1][1]=0;
        PTQueue[Frame_Number-1][0]=index;
        PTQueue[Frame_Number-1][1]=0;
        PTQueueLoseSecondChance(PTQueue); // PTQueue'ya giderek sanslari geri aldim LRU icin
    }
}

void TLBye_yerlestir(int TLB[10][2],int page_table[Frame_Number][2],int index,int TLBQueue[10]){
    int flag=0;
    int flag2=0;
   for (int i = 0; i < 10; i++){
        if(TLB[i][0]==(-1)){
            flag = i;  //tlbde ilk bos yer
            flag2 = 1;
            break;
        }
    }
    
    if(flag2>0) {//tlbde yer var
          TLB[flag][0] = index;
          TLB[flag][1] = index;
        for (int i = 0; i < 10; i++)
            if(TLBQueue[i]==-1){
                TLBQueue[i]=index;
                break;
            }
    }
    else { //tlbde yer yoksa ilk giren cikar
        for (int i = 0; i < 10; i++)
            if (TLBQueue[0] == TLB[i][0])
            {
                TLB[i][0]=index;
                TLB[i][1]=index;
                break;
            }
        TLBQueueUpdate(TLBQueue);
        TLBQueue[9]=index;
    }
}

int main(int argc, char const *argv[]){

    FILE *file=fopen(argv[1],"r");
    FILE *ref=fopen(argv[2],"r");

    if(file == NULL)
    {
        printf("Error! File Could not open !");
        exit(1);
    }
    char dummy;
    while (fscanf (file, "%c", &dummy)!=EOF)  // veri okunuyor dosyadan okunuyor
        N++;
    N=N/2048;

    int page_table[Frame_Number][2];
    int TLB[10][2];
    int TLBQueue[10];
    int PTQueue[Frame_Number][2];

    fclose(file);

    if(ref == NULL)
    { 
        printf("Error! File Could not open ! ");
        exit(1);
    }

    int index=0; //page no
    int tlbRes,ptRes;

    for(int i=0;i<Frame_Number;i++){
        page_table[i][0]=i;
        page_table[i][1]=1;
        PTQueue[i][0]=i;
        PTQueue[i][1]=0;
    }
    for (int i = 0; i < 10; i++)
    {
        TLB[i][0]=-1;
        TLB[i][1]=-1;
        TLBQueue[i]=-1;
    }

    while (fscanf (ref, "%d", &index)!=EOF)
    {
        tlbRes=TLBde_var_mi(TLB,index);
        if(tlbRes==0){ //yok
            toplam_miss++;
            ptRes=PTde_var_mi(page_table,index); // Page Table'da var mi ?
                if(ptRes) // Page Fault Yok
                {
                    PTQueueGiveSecondChance(PTQueue,index); // PTQueue'ya giderek sans verdim LRU icin
                    usleep(10);
                }
                else{
                    toplam_page_fault++;
                    PTye_yerlestir(page_table,index,PTQueue);
                    usleep(100);
                }
                TLBye_yerlestir(TLB,page_table,index,TLBQueue);
        }
        else{
            usleep(1);
            toplam_hit++;
        }
    }
    printf("Toplam hit sayisi : %d \n",toplam_hit);
    printf("Toplam miss sayisi : %d, orani %f\n",toplam_miss,(double)toplam_miss/(toplam_miss+toplam_hit));
    printf("Toplam page fault sayisi: %d, orani %f\n",toplam_page_fault,(double)toplam_page_fault/(toplam_miss+toplam_hit));
    printf("Toplam delay suresi: %dms \n",(toplam_page_fault*100)+((toplam_miss+toplam_hit-toplam_page_fault)*10));
    fclose(ref);
    return 0;
}

