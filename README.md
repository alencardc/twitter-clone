## Como compilar
Para compilar esse projeto é necessário instalar a biblioteca ncurses na versão 6.2.
Isso pode ser feito através do comando `sudo apt-get install libncurses-dev`.

Em seguida, basta executar o comando `make` na raiz do projeto.

## Executar a aplicação
Após a compilação, para executar a aplicação servidor deve ser executado o comando:
`./build/app_server <ip> <porta>`
Exemplo usado nos testes: `./build/app_server 0.0.0.0 8080`

Para executar a aplicação cliente deve ser inserido o IP e porta do servidor:
`./build/app_client <ip> <porta> <username>`
Exemplo usado nos testes:
`./build/app_client 0.0.0.0 8080 @ufrgs`
O usuário deve iniciar com @ e conter entre 4 e 20 caracteres.