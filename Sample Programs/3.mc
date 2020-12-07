bool isPrime(int n){
	if(n==1){
		return false;
	}
	else{
		int i;
		for(i=2;(i*i)<=n;i++){
			if(n%i==0){
				return false;
			}
		}
	}
	return true;
}
int main(){

	int n = readInt();
	int i;

	int ans=0;
	for(i=1;i<n;i++){
		if(isPrime(i)){
			ans=ans+i;
		}
	}
	print(ans, '\n');

	return 0;
}