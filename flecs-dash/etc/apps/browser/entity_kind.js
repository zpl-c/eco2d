
Vue.component('entity-kind', {
    props: [ "kind" ],
    methods: {
      css() {
        var result = "entity-kind";
        if (this.kind && this.kind.length) {
            result += " entity-kind-" + this.kind;
        }
        return result;
      },
    },
    computed: {
        kind_string: function() {
            if (this.kind == "Module") {
                return "m";
            } else if (this.kind == "Component") {
                return "c";
            } else if (this.kind == "System") {
                return "s";
            } else if (this.kind == "Type") {
                return "t";
            } else if (this.kind == "Prefab") {
                return "p";
            } else {
                return "e";
            }
        }
    },
    template: `
      <span :class="css()">
        {{ kind_string }}
      </span>
      `
  });
  