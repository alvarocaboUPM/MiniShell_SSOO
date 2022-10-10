# Version Control for Minishell
## 1.0. Minishell básica
#### 8/10/22
Se ha conseguido una shell que reconoce:

- El número de mandatos introducidos
- El número de argumentos de ellos
- Salida de error en color rojo
- Catch Ctrl+C y Ctrl+\
- Exit con Ctrl+D con mensaje personalizado

Además se ha implementado un promt que reconoce el path actual
con el esquema:
> username@PATH $

#### 9/10/22
Se ejecutan comandos externos (usr/bin) correctamente,
obteniendo su PID del fork con su proceso padre, que les espera para morir

- Se implementa exeIC(command, argc) para distinguir entre funciones internas y externas
- Se crean distintas funciones para gestionar los distintos comandos internos:
    - cd [YES]
    - umask
    - time
    - read