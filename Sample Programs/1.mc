int power(int x, int y){
	int ans=1;
	while(y>0){
		if(y%2==1){
			ans=ans*x;
		}
		x=x*x;
		y=y/2;
	}
	
	return ans;
}

int g(int n, int k){
	int ans=0;
	int i;
	for(i=1;i<=n;i++){
		ans = ans+power(i, k);
	}

	return ans;
}

int main(){
	int n = readInt();
	int k = readInt();
	
	print(g(n, k), '\n');
	return 0;
}