/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of web interface frontend for oai
 * \info component web interface implementation (works with info.component.html)
 */

import {Component} from "@angular/core";
import {IArgType, IInfo} from "src/commondefs";
import {ViewEncapsulation} from "@angular/core";
import {UntypedFormArray} from "@angular/forms";
import {Observable} from "rxjs";
import {filter, map, switchMap, tap} from "rxjs/operators";
import {InfoApi} from "src/app/api/info.api";
import {InfoCtrl} from "src/app/controls/info.control";
import {ModuleCtrl} from "src/app/controls/module.control";
import {VarCtrl} from "src/app/controls/var.control";
import {DialogService} from "src/app/services/dialog.service";
import {DownloadService} from "src/app/services/download.service";

    @Component({
      selector : "app-info",
      templateUrl : "./info.component.html",
      styleUrls : [ "./info.component.scss" ],
      encapsulation : ViewEncapsulation.None,
    }) export class InfoComponent {

  infos$: Observable<VarCtrl[]>;



  constructor(
      public infoApi: InfoApi,
      public downloadService: DownloadService,

  )
  {
    this.infos$ = this.infoApi.readInfos$().pipe(map((infos) => infos.map(info => new InfoCtrl(info))));
  }


  onInfoSubmit(control: InfoCtrl)
  {
    let info: IInfo = control.api();

    if (info.type === IArgType.configfile) {
      this.downloadService.getFile(info.value)
    }
  }

}
