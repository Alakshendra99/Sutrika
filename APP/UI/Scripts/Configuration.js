
document.querySelectorAll("#Home-Call").forEach(card => {
  card.addEventListener("click", () => {
    const page = card.dataset.page;
    window.location.href = page;
  });
});



const fs=require("fs");
const path=require("path");

const ConfigPath=path.join(__dirname,"../Library/Settings.json");

const Form=document.getElementById("ConfigForm");

function LoadConfig(){
  try{
    const Raw=fs.readFileSync(ConfigPath,"utf-8");
    const Config=JSON.parse(Raw);

    document.getElementById("CAN_SPEED").value=Config.CAN.SPEED;

    document.getElementById("REQ_TYPE").value=Config.DoCAN.CANID.REQUEST.TYPE;
    document.getElementById("REQ_ID").value=Config.DoCAN.CANID.REQUEST.ID;

    document.getElementById("RES_TYPE").value=Config.DoCAN.CANID.RESPONSE.TYPE;
    document.getElementById("RES_ID").value=Config.DoCAN.CANID.RESPONSE.ID;

    document.getElementById("FUNC_TYPE").value=Config.DoCAN.CANID.FUNCTIONAL.TYPE;
    document.getElementById("FUNC_ID").value=Config.DoCAN.CANID.FUNCTIONAL.ID;

    document.getElementById("TIMEOUT").value=Config.DoCAN.TIMEOUT;
    document.getElementById("PADDING").value=Config.DoCAN.PADDING;
    document.getElementById("STMIN").value=Config.DoCAN.STMIN;
  }
  catch(Error){
    console.error("Config Load Failed:",Error);
  }
}

function SaveConfig(Event){
  Event.preventDefault();

  const Config={
    CAN:{
      SPEED:document.getElementById("CAN_SPEED").value
    },
    DoCAN:{
      CANID:{
        REQUEST:{
          TYPE:document.getElementById("REQ_TYPE").value,
          ID:document.getElementById("REQ_ID").value
        },
        RESPONSE:{
          TYPE:document.getElementById("RES_TYPE").value,
          ID:document.getElementById("RES_ID").value
        },
        FUNCTIONAL:{
          TYPE:document.getElementById("FUNC_TYPE").value,
          ID:document.getElementById("FUNC_ID").value
        }
      },
      TIMEOUT:Number(document.getElementById("TIMEOUT").value),
      PADDING:document.getElementById("PADDING").value,
      STMIN:document.getElementById("STMIN").value
    }
  };

  try{
    fs.writeFileSync(ConfigPath,JSON.stringify(Config,null,2));
    alert("Configuration Saved");
  }
  catch(Error){
    console.error("Config Save Failed:",Error);
  }
}

Form.addEventListener("submit",SaveConfig);

LoadConfig();