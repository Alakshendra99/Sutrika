const SingleCAN = document.getElementsByClassName("RawSingleCAN");
const RangeCAN = document.getElementsByClassName("RawRangeCAN");
const RawCANMode = document.getElementById("RawCANMode");

function UpdateRawCANWorkMode () {
  if (RawCANMode.value === "single") {
    for (let i = 0; i < SingleCAN.length; i++) { SingleCAN[i].removeAttribute("disabled"); }
    for (let i = 0; i < RangeCAN.length; i++) { RangeCAN[i].setAttribute("disabled",true); }
  } else {
    for (let i = 0; i < SingleCAN.length; i++) { SingleCAN[i].setAttribute("disabled",true); }
    for (let i = 0; i < RangeCAN.length; i++) { RangeCAN[i].removeAttribute("disabled"); }
  }
}

RawCANMode.addEventListener("change", UpdateRawCANWorkMode);
UpdateRawCANWorkMode();