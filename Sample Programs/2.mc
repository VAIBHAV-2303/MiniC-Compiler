int main(){

	int n = readInt();
	int i;

	int flag=0;
	if(n==1){
		flag=1;
	}
	else{
		for(i=2;(i*i)<=n;i++){
			if(n%i==0){
				flag=1;
			}
		}
	}

	if(flag==1){
		print('N', 'O', '\n');
	}
	else{
		print('Y', 'E', 'S', '\n');
	}

	return 0;
}