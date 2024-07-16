

// Bad code, please fix
void 
link(char** tokens, int n)
{
		int i;
		for(i=0;i<n;i++)
		{
				// Also really bad code here!
				if(strcmp(tokens[i], "💬") == 0) { // For echo 🗣️, 
																					 // for echo with WavY eFfeCTs 😱
																					 // For redirection 👉,
						tokens[i] = (char*)"echo";
				} 
				else if(strcmp(tokens[i], "😺") == 0) {
						tokens[i] = (char*)"cat";
				}
				else if(strcmp(tokens[i], "👀") == 0) {
						tokens[i] = (char*)"ls";	
				}
				else if(strcmp(tokens[i], "📦") == 0) {
						tokens[i] = (char*)"cd";	
				}
				else if(strcmp(tokens[i], "📍") == 0) {
						tokens[i] = (char*)"pwd";	
				}
				else if(strcmp(tokens[i], "👉") == 0) {
				
				}
		}

}
