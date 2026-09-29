/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of web interface frontend for oai
 * \api's definitions for the info module, which provides global info about the connected oai modem
 */

import {HttpClient} from "@angular/common/http";
import {Injectable} from "@angular/core";
import {environment} from "src/environments/environment";
import {route, IInfo} from "src/commondefs";

@Injectable({
  providedIn : "root",
})
export class InfoApi {
  constructor(private httpClient: HttpClient)
  {
  }

  public readInfos$ = () => this.httpClient.get<IInfo[]>(environment.backend + route + "info/");
}
