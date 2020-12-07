char a[103];
char st[100][103];

bool match(int ind){

	int i=0;
	int j=0;
	while(a[i]!='.' || st[ind][j]!='.'){
		if(a[i]!=st[ind][j]){
			return false;
		}
		i++;
		j++;
	}
	return true;
}

int main(){
	
	int l=0;
	int flag;
	int j;
	int i;

	readString(a);
	while(a[0]!='.'){
		
		flag=0;
		for(i=0;i<l;i++){
			if(match(i)){
				flag=1;
				break;
			}
		}

		if(flag==0){
			for(j=0;a[j]!='.';j++){
				st[l][j]=a[j];
			}
			st[l][j]='.';
			l++;
		}

		readString(a);
	}
	
	for(i=0;i<l;i++){
		for(j=0;st[i][j]!='.';j++){
			print(st[i][j]);
		}
		print('\n');
	}
	return 0;
}