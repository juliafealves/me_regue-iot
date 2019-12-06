# Me Regue IoT

[pt-br]

## Calculo da Umidade do Solo

Quanto maior o numero lido do ADC, menor a umidad. Sendo assim, calcula-se a porcentagem de umidade por:
```     
Valor lido                 Umidade percentual
  _    0                           _ 100
  |                                |   
  |                                |   
  -   ValorADC                     - UmidadePercentual 
  |                                |   
  |                                |   
 _|_  978                         _|_ 0
```

`UmidadePercentual-0) / (100-0)  =  (ValorADC - 978) / (-978)`

Logo:

`UmidadePercentual = 100 * ((978-ValorADC) / 978)`
