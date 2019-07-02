[@react.component]
let make = (~title, ~isActive, ~onClose, ~children) => {
  Utils.useKeyDown((e: Dom.keyboardEvent) =>
    switch (Webapi.Dom.KeyboardEvent.code(e)) {
    | "Escape" => onClose()
    | _ => ()
    }
  );

  <div className={"modal" ++ (isActive ? " is-active" : "")}>
    <div className="modal-background" />
    <div className="modal-card">
      <header className="modal-card-head">
        <p className="modal-card-title"> title->React.string </p>
        <button
          className="delete"
          ariaLabel="close"
          onClick={_ => onClose()}
        />
      </header>
      <section className="modal-card-body"> children </section>
    </div>
  </div>;
};