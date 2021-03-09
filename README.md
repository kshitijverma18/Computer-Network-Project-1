# Computer-Network
This is a socket program written in C without the use of high level libraries like Curl to download a webpage through Proxy. The program creates a socket between the device
and the Squid Proxy server using the IP address and port number of the proxy server. The proxy requires username and password authentication in base 64 encoded format. 
The GET request is then sent to the proxy to download the webpage. The header message is read and checked for redirection. If the re-direction is required, the absolute 
location of the new redirected webpage is generated and the message to fetch webpage from the new redirected location is again sent. The code also downloads the logo of 
a particular webpage http://info.in2p3.fr/

#### The Code takes the following arguments:
1. Name of the Webpage (Without http): 
2. Proxy IP address
3. Proxy Port Number
4. Username for Proxy Authenitcation
5. Password for Proxy Authentication
6. Name of the downloaded html file
7. Name of the downloaded logo
## Usage
#### Compile using : <pre><code>gcc -o http_proxy_download http_proxy_download.c</code></pre>

#### To Run the code: <pre><code>./http_proxy_download.out info.in2p3.fr 182.75.45.22 13128 csf303 csf303 index.html logo.gif</code></pre>

