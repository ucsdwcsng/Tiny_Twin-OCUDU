/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of web interface frontend for oai
 */

import {HttpClient} from "@angular/common/http";
import {Injectable} from "@angular/core";
import {environment} from "src/environments/environment";
import {route, IArgType, IInfo} from "src/commondefs";
export interface IModule {
  name: string;
}

export enum ILogLvl {
  error = "error",
  warn = "warn",
  analysis = "analysis",
  info = "info",
  debug = "debug",
  trace = "trace"
}

export enum ILogOutput {
  stdout = "stdout",
  telnet = "telnet",
  web = "web",
  file = "/tmp/<component>.log",
}

export enum ICommandOptions {
  update = "update", // result can be updated, triggers update button on result page when set
  help = "help" // help tooltip available on command buttons
}

export interface IVariable {
  name: string;
  value: string;
  type: IArgType;
  modifiable: boolean; // set command ?
}

export interface ICommand {
  name: string;
  confirm?: string;
  question?: IQuestion[];
  param?: IVariable[];
  options?: ICommandOptions[];
}
export interface ITable {
  columns: IColumn[];
  rows: string[];
}
export interface IQuestion {
  display: string;
  pname: string;
  type: IArgType;
}
export interface IResp {
  display: string[], table?: ITable
}

export interface IParam {
  value: string, col: IColumn
}

export interface IColumn { // should use IVariable ?
  name: string;
  type: IArgType;
  modifiable: boolean; // set command ?
  help: boolean; // is help available
}
export interface IRow {
  params: IParam[], rawIndex: number, cmdName: string,
      param?: IVariable // to transmit the initial command parameter, ex: the channel model index when modify a channel model
}


@Injectable({
  providedIn : "root",
})
export class CommandsApi {
  constructor(private httpClient: HttpClient)
  {
  }

  public readModules$ = () => this.httpClient.get<IModule[]>(environment.backend + route + "commands/");

  public readVariables$ = (moduleName: string) => this.httpClient.get<IInfo[]>(environment.backend + route + moduleName + "/variables/");

  public readCommands$ = (moduleName: string) => this.httpClient.get<ICommand[]>(environment.backend + route + moduleName + "/commands/");

  public runCommand$ = (command: ICommand, moduleName: string) => this.httpClient.post<IResp>(environment.backend + route + moduleName + "/commands/", command);

  public setCmdVariable$ = (variable: IInfo, moduleName: string) => this.httpClient.post<IResp>(environment.backend + route + moduleName + "/variables/", variable);

  public setCmdParams$ = (row: IRow, moduleName: string) => this.httpClient.post<IResp>(environment.backend + route + moduleName + "/set/", row);
}
