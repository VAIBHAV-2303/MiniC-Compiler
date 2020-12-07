int main(){

	char a[103];
	int freq[26];
	
	int i;
	for(i=0;i<26;i++){
		freq[i]=0;
	}

	readString(a);

	i=0;	
	while(a[i]!='.'){
		freq[(int)a[i]-97]++;
		i++;
	}

	for(i=0;i<26;i++){
		print((char)(97+i), ' ', freq[i], '\n');
	}
	return 0;
}