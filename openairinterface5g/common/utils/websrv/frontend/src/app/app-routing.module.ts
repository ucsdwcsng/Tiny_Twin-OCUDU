/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of web interface frontend for oai
 */

import {NgModule} from "@angular/core";
import {RouterModule, Routes} from "@angular/router";

import {AppComponent} from "./app.component";
import {CommandsComponent} from "./components/commands/commands.component";

const routes: Routes = [
  {path : "", redirectTo : "/websrv", pathMatch : "full"},
  {path : "websrv", component : AppComponent},
  {path : "**", redirectTo : ""},
];

@NgModule({
  imports : [ RouterModule.forRoot(routes, {relativeLinkResolution : "legacy"}) ],
  exports : [ RouterModule ],
})
export class AppRoutingModule {
}
