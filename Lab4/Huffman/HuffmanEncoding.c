#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct node *rptr=NULL;

struct node{
	struct node *lcptr;
	char sym;
	int freq;
	struct node *rcptr;
};

struct lookup{
	char s;
	char code[200];
};



char s[200];
char ds[200];
int f[200];
int n;
int nc;
struct lookup LookUpTable[200];
int lookup_size;




////////////////////////////////////////////////////

void ISort(){
	for(int i=1;i<nc;i++){
		for(int j=i;j>0;j--){
			if(f[j-1]>f[j]){
				int temp=f[j];
				f[j]=f[j-1];
				f[j-1]=temp;
				
				char temp2=ds[j];
				ds[j]=ds[j-1];
				ds[j-1]=temp2;
			}
			else break;
		}
	}
}

////////////////////////////////////////////////////

void Read(){
	printf("Enter the string: ");
	scanf("%s", s);
	n = strlen(s);
	lookup_size = 0;
}

void count(){
	nc=0;
	for(int i=0;i<n;i++){
		int flag=0;
		//ds[0]=0;
		for(int j=0;j<nc;j++){
			if(s[i]==ds[j]){
				flag=1;
				break;
			}
		}
		if(flag==0 ){
			ds[nc]=s[i];
			nc++;
		}
	}
	for(int i=0;i<nc;i++){
		int cou=0;
		for(int j=0;j<n;j++){
			if(ds[i]==s[j]){
				cou++;
			}
		}
		f[i]=cou;
	}
	
	ISort();

}

void BuildTree(int a[], char c[], int size, struct node *ptrarr[]){
	if(size>1){
		int b[200];
		char t[200];
		struct node *nparr[200];
		struct node *ptr = malloc(sizeof(struct node));
		
		
		int temp=a[0]+a[1];
		int tempi=0;
		
		for(int i=2;i<size;i++){
			if(temp>=a[i]){
				tempi=i-1;
			}
		}
		//printf("\n\ntempi - %d -- temp - %d\n\nSize - %d\n\n", tempi, temp, size);
		int ct=2;
		for(int i=0;i<tempi;i++){
			b[i]=a[ct];
			t[i]=c[ct];
			nparr[i]=ptrarr[ct];
			ct++;
		}
		b[tempi]=temp;
		nparr[tempi]=ptr;
		t[tempi]=0;
		for(int i=tempi+1;i<nc-1;i++){
			b[i]=a[ct];
			t[i]=c[ct];
			nparr[i]=ptrarr[ct];
			ct++;
		}
		
		struct node *lfptr = malloc(sizeof(struct node));
		struct node *rgptr = malloc(sizeof(struct node));
		
		lfptr->sym=c[0];
		lfptr->freq = a[0];
		
		if(ptrarr[0]!=NULL){
			lfptr = ptrarr[0];
		}
		
		rgptr->sym=c[1];
		rgptr->freq = a[1];
		
		if(ptrarr[1]!=NULL){
			rgptr = ptrarr[1];
		}
		
		ptr->sym=0;
		ptr->freq=temp;
		ptr->lcptr=lfptr;
		ptr->rcptr=rgptr;
		rptr=ptr;

		BuildTree(b,t,size-1,nparr);
	}
}

void LookupTable(struct node *ptr, char en[], int ensize){
	if(ptr!=NULL){
		if(ptr->lcptr==NULL && ptr->rcptr==NULL){
			lookup_size++;
			int index = (int)(ptr->sym);
			LookUpTable[index].s = ptr->sym;
			for(int i=0;i<ensize;i++)LookUpTable[index].code[i] = en[i];
		}
		else {
			ensize++;
			en[ensize-1] = '0';
			LookupTable(ptr->lcptr, en, ensize);
			
			en[ensize-1] = '1';
			LookupTable(ptr->rcptr, en, ensize);
		}
	}
}

void Encoder(){
	printf("Encoded Sequence: ");
	char temp[400];
	for(int i=0;i<n;i++){
		int index = (int)(s[i]);
		for(int j=0;j<strlen(LookUpTable[index].code);j++) printf("%c", LookUpTable[index].code[j]);
		//printf("|");
	}
	printf("\n");
}

void Decoder(char seq[], int nd){
	int cn=0;
	printf("Decoded sequence: ");
	while(cn<nd){
		struct node *ptr = rptr;
		while(ptr!=NULL){
			if(ptr->lcptr==NULL && ptr->rcptr==NULL){
				printf("%c", ptr->sym);
				ptr=NULL;
			}
			else {
				if(seq[cn]=='0')ptr=ptr->lcptr;
				else if(seq[cn]=='1')ptr=ptr->rcptr;
				cn++;
			}
			
		}
	}
	printf("\n");
}

void print(struct node *ptr){
	if(ptr!=NULL){
		print(ptr->lcptr);
		printf("\n");
		printf("%c - %d", ptr->sym, ptr->freq);
		printf("\n");
		print(ptr->rcptr);
	}
}



int main(){
	printf("----------------------Huffman Coding-----------------------\n");
	
	rptr=NULL;
	
	//printf("Enter 0 for encoding and 1 for decoding: ");
	int tt=0;
	//scanf("%d", &tt);
	if(tt==0){
		Read();
		count();
		for(int i=0;i<nc;i++){
			printf("%c - %d\n", ds[i], f[i]);
		}
		struct node *ptrarr[200];
		for(int i=0;i<nc;i++)ptrarr[i]=NULL;
		BuildTree(f,ds,nc,ptrarr);
		printf("\n\n");
		print(rptr);
		printf("\n------------Encode----------\n");
		char en[200];
		LookupTable(rptr, en, 0);
		Encoder();
		printf("\n----------------------------\n");
		tt++;
	}
	if(tt==1) { //REVERT TO 1
		printf("\n------------Decode----------\n");
		printf("Enter the sequence: ");
		char seq[200];
		scanf("%s", seq);
		Decoder(seq, strlen(seq));
		printf("\n----------------------------\n");
	}
	
	
	printf("---------------------------------------------------------\n");
}



///////////////////////////////////////////////////////

