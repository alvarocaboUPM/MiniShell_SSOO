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
    - cd [OK]
    - umask [YES] (20/10)
    - time
    - read

## 1.1. Implementamos Redirs, Pipes y Background
#### 20/10/22
Objetivos de esta nueva versión es permitir que el usuario redireccione tanto la salida como la entrada estandar.
Posteriormente, se intentará implementar pipes

#### 29/10/22
- Intento implementar pipes:
    * A