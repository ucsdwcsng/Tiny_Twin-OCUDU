/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*! 
 * \brief: implementation of web interface frontend for oai
 * \utility component used to allow entering parameters before sending a request to the backend
 */

import {Component, Inject} from "@angular/core";
import {MAT_DIALOG_DATA, MatDialogRef} from "@angular/material/dialog";
import {CmdCtrl} from "src/app/controls/cmd.control";
import {HelpApi, HelpRequest, HelpResp} from "src/app/api/help.api";
export interface QuestionDialogData {
  title: string;
  control: CmdCtrl;
}
@Component({selector : "app-question", templateUrl : "./question.component.html", styleUrls : [ "./question.component.css" ]})
export class QuestionDialogComponent {
  hlp_question: string = "";
  constructor(
      public dialogRef: MatDialogRef<QuestionDialogComponent>,
      public helpApi: HelpApi,
      @Inject(MAT_DIALOG_DATA) public data: QuestionDialogData,
  )
  {
  this.helpApi.getHelpText("question", data.control.cmdname, "input").subscribe( response => { this.hlp_question = response;})
  }
  
  
  onNoClick()
  {
    this.dialogRef.close();
  }
}
