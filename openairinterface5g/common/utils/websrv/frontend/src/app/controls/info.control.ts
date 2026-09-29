/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of web interface frontend for oai
 * \implement one info item for info component
 */
import {UntypedFormControl, UntypedFormGroup} from "@angular/forms";
import {IArgType, IInfo} from "src/commondefs";

const enum InfosFCN {
  name = "name",
  value = "value",
  type = "type",
  modifiable = "modifiable"
}

export class InfoCtrl extends UntypedFormGroup {
  type: IArgType;
  constructor(ivar: IInfo)
  {
    super({});
    this.type = ivar.type;
    this.addControl(InfosFCN.name, new UntypedFormControl(ivar.name));
    this.addControl(InfosFCN.value, new UntypedFormControl(ivar.value));
    this.addControl(InfosFCN.type, new UntypedFormControl(ivar.type));
    this.addControl(InfosFCN.modifiable, new UntypedFormControl(ivar.modifiable));
  }

  api()
  {
    const doc: IInfo = {
      name : this.nameFC.value,
      value : String(this.valueFC.value), // FIXME
      type : this.typeFC.value,
      modifiable : this.modifiableFC.value
    };

    return doc;
  }

  get nameFC()
  {
    return this.get(InfosFCN.name) as UntypedFormControl;
  }

  set nameFC(control: UntypedFormControl)
  {
    this.setControl(InfosFCN.name, control);
  }

  get valueFC()
  {
    return this.get(InfosFCN.value) as UntypedFormControl;
  }

  set valueFC(control: UntypedFormControl)
  {
    this.setControl(InfosFCN.value, control);
  }

  get typeFC()
  {
    return this.get(InfosFCN.type) as UntypedFormControl;
  }

  set typeFC(control: UntypedFormControl)
  {
    this.setControl(InfosFCN.type, control);
  }

  get modifiableFC()
  {
    return this.get(InfosFCN.modifiable) as UntypedFormControl;
  }

  set modifiableFC(control: UntypedFormControl)
  {
    this.setControl(InfosFCN.modifiable, control);
  }

  get btnTxtFC()
  {
    if (this.type != IArgType.configfile)
      return "set"
      else return "download"
  }
}
