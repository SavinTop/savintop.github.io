const dataOut = document.getElementById("dataOutput");

let GeneralRecordsInfo = {};

let Records = {};

const RecordState = {
    IDLE: 0,
    SENDING_DATA: 1,
    EDITING: 2
};

class Record {
    constructor(id, keyInd, keyVal) {
        this.state = RecordState.IDLE;
        this.id = id;
        this.keyVal = keyVal;
        this.keyInd = keyInd;
        this.dataBackup = [];

        this.recordId = "record_" + id;
        this.loadingId = this.recordId + "_loading";
        this.editBtnId = this.recordId + "_editBtn";
        this.inputsName = this.recordId + "_inputs"
    }

    LoadingAnimation(bool) {
        $('#' + this.loadingId).attr("class", bool ? "spinner" : "");
    }

    deleteRec() {
        let xhttp = new XMLHttpRequest();

        let this_ = this;

        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                let obj = JSON.parse(this.responseText);

                if (obj["Error"] != null) {
                    alert(obj["Error"]);
                    this_.state = RecordState.IDLE;
                    this_.LoadingAnimation(false); return;
                }

                document.getElementById(this_.recordId).remove();
                delete Records[this_.id];
            }
        };
        xhttp.open("DELETE", "_bd/" + GeneralRecordsInfo["TableName"] + '/' + this.keyVal, true);
        xhttp.send();
        this.state = RecordState.SENDING_DATA;
        this.LoadingAnimation(true);
    }

    editingStarted() {
        if (this.state != RecordState.IDLE) return;
        let editBtn = document.getElementById(this.editBtnId);
        let inputs = document.getElementsByName(this.inputsName);

        this.dataBackup = [];

        inputs.forEach((el, i) => {
            if (i != this.keyInd)
                el.disabled = false;
            this.dataBackup.push(el.value);
        });
        editBtn.className = "btn btn-success";
        editBtn.innerText = "OK";
        this.state = RecordState.EDITING;
    }

    editingDone() {
        if (this.state != RecordState.EDITING) return;

        let editBtn = document.getElementById(this.editBtnId);
        let inputs = document.getElementsByName(this.inputsName);

        editBtn.className = "btn btn-primary";
        editBtn.innerText = "edit";

        let editQuery = { "row": [], "columns": [] };

        for (let i = 0; i < GeneralRecordsInfo["Columns"].length; i++) {
            let el = GeneralRecordsInfo["Columns"][i];
            if (i != this.keyInd)
                editQuery["columns"].push(el["Name"]);
        }

        inputs.forEach((el, i) => {
            if (i != this.keyInd)
                editQuery["row"].push(el.value);
            el.disabled = true;
        });

        this.state = RecordState.SENDING_DATA;

        let this_ = this;

        let xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                let obj = JSON.parse(this.responseText);

                if (obj["Error"] != null) {
                    alert(obj["Error"]);
                    this_.recoverEditData();
                    this_.LoadingAnimation(false);
                    this_.state = RecordState.IDLE;
                    return;
                }


                this_.LoadingAnimation(false);
                this_.state = RecordState.IDLE;
            }
        };
        xhttp.open("PATCH", "_bd/" + GeneralRecordsInfo["TableName"] + '/' + this.keyVal, true);
        xhttp.setRequestHeader("Content-Type", "application/json;charset=UTF-16");
        xhttp.send(JSON.stringify(editQuery));
        this.LoadingAnimation(true);
    }

    recoverEditData() {
        let inputs = document.getElementsByName(this.inputsName);

        this.dataBackup.forEach((el, i) => {
            inputs[i].value = el;
        });
    }

}

function generateDrops() {
    let arr = {};

    GeneralRecordsInfo["ForeignInds"].forEach(element => {
        let out = "";

        let currentCol = GeneralRecordsInfo["Columns"][element];

        let cols = currentCol["RefTable"]["Columns"];
        let rows = currentCol["RefTable"]["Rows"];
        let columnTo = currentCol["ColumnTo"];

        let colIndex = index = cols.findIndex(x => x == columnTo);

        out +=/* html */`
            <div class="dropdown">
            <button class="btn btn-secondary dropdown-toggle" type="button" id="dropdownMenuButton" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
            </button>
            <div class="dropdown-menu" aria-labelledby="dropdownMenuButton">
            <table class="table">
                <thead>
                    <tr>`;

        for (let i = 0; i < cols.length; i++)
            out +=/* html */`<th scope="col">${cols[i]}</th>`;

        out +=/* html */`      
                    </tr>
                </thead>
                <tbody>`;
        if (rows != null) {
            for (let i = 0; i < rows.length; i++) {
                out +=/* html */`<tr class="iJustNeed">`

                for (let j = 0; j < rows[i].length; j++)
                    if (j != colIndex)
                        out +=/* html */`<td>${rows[i][j]}</td>`;
                    else
                        out +=/* html */`<th scope="row">${rows[i][j]}</th>`;

                out +=/* html */`</tr>`;
            }
        }

        out +=/* html */`
            </table>
            </div>
            </div>`

        arr[element] = out;
    });

    return arr;
}

function loadTableData(name) {
    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let obj = JSON.parse(this.responseText);

            GeneralRecordsInfo["TableName"] = name;

            let arr = {};
            let foreignArr = [];

            obj["Columns"].forEach((el, i) => {
                let temp = obj["ColumnsInfo"][el];
                temp["Name"] = el;

                if (temp["ForeignKey"] == true) foreignArr.push(i);
                else
                    if (temp["PrimaryKey"] == true) GeneralRecordsInfo["PrimaryKey"] = el;

                arr[i] = temp;
            });
            arr.length = obj["Columns"].length;
            GeneralRecordsInfo["Columns"] = arr;
            GeneralRecordsInfo["ForeignInds"] = foreignArr;
            GeneralRecordsInfo["Rows"] = obj["Rows"];

        }
    };
    xhttp.open("GET", "_bd/" + name, false);
    xhttp.send();
}

function loadNewTable(name) {
    loadTableData(name);
    let out = "";

    let foreignArr = generateDrops();

    if (GeneralRecordsInfo["Rows"] == null) {
        Rect.Render(dataOut, createAddingForm(foreignArr));
        return;
    }

    let keyIndex;

    for (let j = 0; j < GeneralRecordsInfo["Columns"].length; j++) {
        let currentCol = GeneralRecordsInfo["Columns"][j];
        if (currentCol["PrimaryKey"] == true) {
            keyIndex = j;
            break;
        }
    }

    out +=/* html */ `<ul class="list-group">`;

    for (let i = 0; i < GeneralRecordsInfo["Rows"].length; i++) {
        out +=/* html */`
        <li class="list-group-item" id="record_${i}">
        <button type="button" class="btn btn-primary" style="float: right;" id="record_${i}_editBtn" onclick="editBtn(${i})">edit</button>
        <button type="button" class="btn btn-danger" style="float: right; " onclick="deleteBtn(${i})">delete</button>\
        <table cols="3" cellpadding="5">
      `;
        let keyValue;

        for (let j = 0; j < GeneralRecordsInfo["Columns"].length; j++) {
            let currentCol = GeneralRecordsInfo["Columns"][j];
            let currentRowEl = GeneralRecordsInfo["Rows"][i][j];

            if (keyIndex == j)
                keyValue = currentRowEl;

            if (currentCol["ForeignKey"] == true) {
                out +=/* html */`
            <tr><td>${currentCol["Name"]} : </td>
            <td><input value = "${currentRowEl}" class="form-control" name="record_${i}_inputs" disabled></td><td>${foreignArr[j]}</td></tr>`;
            } else
                out +=/* html */`
            <tr><td>${currentCol["Name"]} : </td>
            <td><input value = "${currentRowEl}" class="form-control" name="record_${i}_inputs" disabled></td></tr>
            `;
        }


        out +=/* html */`</table><div class="" id="record_${i}_loading"></div></li>`;

        Records[i] = new Record(i, keyIndex, keyValue);
    }

    out += '</ul>' + createAddingForm(foreignArr);

    Rect.Render(dataOut, out);
}

function deleteBtn(RecordId) {
    Records[RecordId].deleteRec();
}

function editBtn(RecordId) {
    if (Records[RecordId].state == RecordState.IDLE)
        Records[RecordId].editingStarted();
    else
        Records[RecordId].editingDone();
}

function createAddingForm(foreignArr) {
    let out = /* html */`
    <li class="list-group-item" style="margin-top:30px">
    <button type="button" class="btn btn-success" style="float: right;" onclick="addRow()">add</button>`;

    out +=/* html */`
        <table cols="3" cellpadding="5">
      `;

    for (let j = 0; j < GeneralRecordsInfo["Columns"].length; j++) {
        let currentCol = GeneralRecordsInfo["Columns"][j];
        if (currentCol["PrimaryKey"] == true)
            continue;

        if (currentCol["ForeignKey"] == true)
            out +=/* html */`
        <tr><td>${currentCol["Name"]} : </td>
        <td><input class="form-control" name="record_add_inputs"></td><td>${foreignArr[j]}</td></tr>`;
        else
            out +=/* html */`
        <tr><td>${currentCol["Name"]} : </td>
        <td><input class="form-control" name="record_add_inputs"></td></tr>
        `;
    }

    out +=/* html */`</table></li>`;

    return out;
}

function addRow() {
    let inputs = document.getElementsByName("record_add_inputs");

    let addQuery = { "row": [], "columns": [] };

    for (let i = 0; i < GeneralRecordsInfo["Columns"].length; i++) {
        let currentCol = GeneralRecordsInfo["Columns"][i];
        if (currentCol["PrimaryKey"] == true)
            continue;

        addQuery["columns"].push(currentCol["Name"]);
    }

    inputs.forEach((el, i) => {
        addQuery["row"].push(el.value);
        el.value = "";
    });

    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let obj = JSON.parse(this.responseText);

            if (obj["Error"] != null) { alert(obj["Error"]); return; }

            loadNewTable(GeneralRecordsInfo["TableName"]);
        }
    };
    xhttp.open("POST", "_bd/" + GeneralRecordsInfo["TableName"], false);
    xhttp.setRequestHeader("Content-Type", "application/json;charset=UTF-16");
    xhttp.send(JSON.stringify(addQuery));
}

