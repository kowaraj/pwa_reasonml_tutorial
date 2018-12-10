module WithRouter = {
  type state = {currentRoute: Config.routes};
  type action =
    | ChangeUrl(Config.routes);
  let component = ReasonReact.reducerComponent("WithRouter");
  let make = children => {
    ...component,
    initialState: () => {
      currentRoute:
        ReasonReact.Router.dangerouslyGetInitialUrl() |> Config.urlToRoute,
    },
    reducer: (action, _state) =>
      switch (action) {
      | ChangeUrl(route) => ReasonReact.Update({currentRoute: route})
      },

    didMount: self => {
      let watcherID = ReasonReact.Router.watchUrl(url =>
            self.send(ChangeUrl(url |> Config.urlToRoute))
          );
          self.onUnmount( ()=>ReasonReact.Router.unwatchUrl(watcherID));
      },
    render: ({state}) => children(~currentRoute=state.currentRoute),
  };
};

module Link = {
  let component = ReasonReact.statelessComponent("Link");
  let make = (~route, ~className="", children) => {
    ...component,
    render: self => {
      let href = Config.routeToString(route);
      ReasonReact.createDomElement(
        "a",
        ~props={
          "className": className,
          "href": href,
          "onClick":
            self.handle((event, _self) => {
              ReactEventRe.Mouse.preventDefault(event);
              ReasonReact.Router.push(href);
            }),
        },
        children,
      );
    },
  };
};

module NavLink = {
  let component = ReasonReact.statelessComponent("NavLink");
  let make = (~route, ~activeRoute=?, ~className=?, children) => {
    ...component,
    render: _self =>
      <WithRouter>
        ...(
             (~currentRoute) => {
               let activeRoute =
                 switch (activeRoute) {
                 | None => route
                 | Some(route) => route
                 };
               let className =
                 Cn.make([
                   Cn.unpack(className),
                   Cn.ifTrue("active", activeRoute == currentRoute),
                 ]);
               <Link route className> ...children </Link>;
             }
           )
      </WithRouter>,
  };
};
