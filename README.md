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

## Configurações do Terminal
A aplicação cliente utiliza uma estrutura e formato padrão, então, para garantir a visualização correta da interface garanta que o terminal possuao *ao menos* 24 linhas e 100 colunas.


## Como navegar na aplicação
*Enviar notificações:* aperte a tecla `c` para pode digitar na caixa de input.
Dentro da caixa de input a tecla `Enter/Return` envia o comando e volta para o estado inicial.
Para cancelar a escrita de um comando, aperte `F1` para retornar ao estado inicial.

*Ler notificações:* aperte a tecla `n` para acessar as tela de notificações. Nessa tela você pode usar as setas para cima e para baixo para navegar na lista de noticações e ver o histórico de notificações recebidas.
Aperte `F1` para retornar ao estado inicial.

*Fechar aplicação:* Para fechar aplicação pode ser utilizado comando `Ctrl+D` ou apertar `F1` quando estiver no estado inicial.
