/**
 * SGN - Robótica Educacional
 * OUT-2017
 *
 * Projeto: Exemplo de datalogger com ESP01 e Arduino UNO
 *
 * Resumo: Transferir para uma planilha do Google Drive os dados
 * recebidos por um comando GET.
 *
 * Link para executar este script:
 * https://script.google.com/macros/s/AKfycbxm_tlU2QHpAuG3BKv-pN4_CkFUqeES_1hXoWkS4TrYaoi9e_c/exec?value=100
 */

/* doGet: Função chamada numa requisição GET */
function doGet(data) {
  try
  {
    var value = ""; 
    /* Armazena o valor do parâmetro em uma variável*/
    value = data.parameters.value;
        
    /* Armazena a URL da planilha */
    var sheetUrl = "https://docs.google.com/spreadsheets/d/1GWFdMUujGt45282k5wxepbCFAMrfky2G0qsyEgGPTwU/edit";
    /* Abre a planilha */    
    var spreadsheet = SpreadsheetApp.openByUrl(sheetUrl);
        
    /** Procura a página selecionada */
    var dataLogSheet = spreadsheet.getSheetByName("Datalog");
        
    /** Obtém a última linha preenchida) */
    var row = dataLogSheet.getLastRow() + 1;
    
    /** Preenche os dados */
    dataLogSheet.getRange("A" + row).setValue(new Date()); // Data/Hora
    dataLogSheet.getRange("B" + row).setValue(value); // valor

    /** Armazena na variável o valor da célula D1, a célula que escolhemos para ser nossa configuração */
    var reference = dataLogSheet.getRange("D1").getValue();
    
    /* Responde um texto qeu adiciona um "ref:" antes do valor e um "$$$" depois. Isso nos ajudará a separar o texto
       no arduino*/ 
    return ContentService.createTextOutput("ref:"+reference+"$$$");
      
  }
  catch(error)
  {
    var err = "oops...." + error.message;
    return ContentService.createTextOutput("oops...." + error.message);
  }
}

